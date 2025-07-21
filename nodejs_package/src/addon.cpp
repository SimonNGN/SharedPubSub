#include <napi.h>
#include "SharedPubSub.hpp"
#include "FixedString.h"
#include "Examples.h"
#include <chrono>

using namespace shps;
using namespace std;

// Forward declaration of utility function
template<typename ReturnType>
class AsyncWorker;

template<typename ReturnType>
Napi::Value createAsyncMethod(const Napi::CallbackInfo& info, 
                                typename AsyncWorker<ReturnType>::WorkFunction work,
                                int callbackIndex = -1);

template<typename T>
remove_atomic_t<T> convertFromJS(const Napi::Value& value);

template<typename T>
Napi::Value convertToJS(Napi::Env env, T& value);

// Publisher wrapper of the original c++ version
template<typename T>
class PublisherWrapper : public Napi::ObjectWrap<PublisherWrapper<T>> {
public:

    PublisherWrapper(const Napi::CallbackInfo& info) 
        : Napi::ObjectWrap<PublisherWrapper<T>>(info),
        publisher(info[0].As<Napi::String>().Utf8Value()) {
    }

    static Napi::Object Init(Napi::Env env, Napi::Object exports, const string& className) {
        Napi::Function func = PublisherWrapper<T>::DefineClass(env, className.c_str(), {
            //PublisherWrapper<T>::InstanceMethod("rawValue", &PublisherWrapper<T>::rawValue),
            PublisherWrapper<T>::InstanceMethod("setValue", &PublisherWrapper<T>::setValue),
            PublisherWrapper<T>::InstanceMethod("readValue", &PublisherWrapper<T>::readValue),
            PublisherWrapper<T>::InstanceMethod("publish", &PublisherWrapper<T>::publish),
            PublisherWrapper<T>::InstanceMethod("push", &PublisherWrapper<T>::push),
            PublisherWrapper<T>::InstanceMethod("setValueAndPush", &PublisherWrapper<T>::setValueAndPush),
            PublisherWrapper<T>::InstanceMethod("publishOnChange", &PublisherWrapper<T>::publishOnChange),
            PublisherWrapper<T>::InstanceMethod("setValueAndNotifyOnChange", &PublisherWrapper<T>::setValueAndNotifyOnChange),
            PublisherWrapper<T>::InstanceMethod("notifyAll", &PublisherWrapper<T>::notifyAll),
        });
        exports.Set(className, func);
        return exports;
    }

private:
    Napi::Value publish(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher.publish(value);
        return info.Env().Undefined();
    }

    Napi::Value publishOnChange(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher.publishOnChange(value);
        return info.Env().Undefined();
    }

    Napi::Value setValue(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher.setValue(value);
        return info.Env().Undefined();
    }

    Napi::Value setValueAndNotifyOnChange(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher.setValueAndNotifyOnChange(value);
        return info.Env().Undefined();
    }

    Napi::Value readValue(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = publisher.readValue();
        return convertToJS(info.Env(), value);
    }

    Napi::Value notifyAll(const Napi::CallbackInfo& info) {
        publisher.notifyAll();
        return info.Env().Undefined();
    }
    Napi::Value push(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher.push(value);
        return info.Env().Undefined();
    }

    Napi::Value setValueAndPush(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher.setValueAndPush(value);
        return info.Env().Undefined();
    }

    Publisher<T> publisher;
};

// Subscriber wrapper of the original c++ version
template<typename T>
class SubscriberWrapper : public Napi::ObjectWrap<SubscriberWrapper<T>> {
public:

    SubscriberWrapper(const Napi::CallbackInfo& info) 
        : Napi::ObjectWrap<SubscriberWrapper<T>>(info),
          subscriber(info[0].As<Napi::String>().Utf8Value(), 
                    info[1].As<Napi::String>().Utf8Value(), 
                    info.Length() > 2 ? info[2].As<Napi::Boolean>().Value() : false) {
    }

    static Napi::Object Init(Napi::Env env, Napi::Object exports, const string& className) {
        Napi::Function func = SubscriberWrapper<T>::DefineClass(env, className.c_str(), {
            SubscriberWrapper<T>::InstanceMethod("subscribe", &SubscriberWrapper<T>::subscribe),
            SubscriberWrapper<T>::InstanceMethod("clearQueue", &SubscriberWrapper<T>::clearQueue),
            //SubscriberWrapper<T>::InstanceMethod("rawValue", &SubscriberWrapper<T>::rawValue),
            SubscriberWrapper<T>::InstanceMethod("readValue", &SubscriberWrapper<T>::readValue),
            SubscriberWrapper<T>::InstanceMethod("readWait", &SubscriberWrapper<T>::readWait),
            SubscriberWrapper<T>::InstanceMethod("readWaitAsync", &SubscriberWrapper<T>::readWaitAsync),
            SubscriberWrapper<T>::InstanceMethod("readWaitMS", &SubscriberWrapper<T>::readWaitMS),
            SubscriberWrapper<T>::InstanceMethod("readWaitMSAsync", &SubscriberWrapper<T>::readWaitMSAsync),
            SubscriberWrapper<T>::InstanceMethod("waitForNotify", &SubscriberWrapper<T>::waitForNotify),
            SubscriberWrapper<T>::InstanceMethod("waitForNotifyAsync", &SubscriberWrapper<T>::waitForNotifyAsync),
            SubscriberWrapper<T>::InstanceMethod("waitForNotifyMS", &SubscriberWrapper<T>::waitForNotifyMS),
            SubscriberWrapper<T>::InstanceMethod("waitForNotifyMSAsync", &SubscriberWrapper<T>::waitForNotifyMSAsync),
        });
        exports.Set(className, func);
        return exports;
    }

private:

    Napi::Value subscribe(const Napi::CallbackInfo& info) {
        bool result = subscriber.subscribe();
        return Napi::Boolean::New(info.Env(), result);
    }

    Napi::Value clearQueue(const Napi::CallbackInfo& info) {
        bool result = subscriber.clearQueue();
        return Napi::Boolean::New(info.Env(), result);
    }

    Napi::Value readValue(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = subscriber.readValue();
        return convertToJS(info.Env(), value);
    }

    Napi::Value readWait(const Napi::CallbackInfo& info) {
        auto result = subscriber.readWait();
        if (!result.has_value()) return info.Env().Null();
        return convertToJS(info.Env(), result.value());
    }

    Napi::Value readWaitMS(const Napi::CallbackInfo& info) {
        long long timeout = info[0].As<Napi::Number>().Int64Value();
        auto result = subscriber.readWait(chrono::milliseconds(timeout));
        if (!result.has_value()) return info.Env().Null();
        return convertToJS(info.Env(), result.value());
    }

    Napi::Value waitForNotify(const Napi::CallbackInfo& info) {
        subscriber.waitForNotify();
        return info.Env().Undefined();
    }

    Napi::Value waitForNotifyMS(const Napi::CallbackInfo& info) {
        long long timeout = info[0].As<Napi::Number>().Int64Value();
        subscriber.waitForNotify(chrono::milliseconds(timeout));
        return info.Env().Undefined();
    }

        // New async methods using the generic worker
    Napi::Value readWaitAsync(const Napi::CallbackInfo& info) {
        auto work = [this]() -> optional<remove_atomic_t<T>> {
            return subscriber.readWait();
        };
        return createAsyncMethod<optional<remove_atomic_t<T>>>(info, work, 0);
    }

    Napi::Value readWaitMSAsync(const Napi::CallbackInfo& info) {
        long long timeout = info[0].As<Napi::Number>().Int64Value();
        auto work = [this, timeout]() -> optional<remove_atomic_t<T>> {
            return subscriber.readWait(chrono::milliseconds(timeout));
        };
        return createAsyncMethod<optional<remove_atomic_t<T>>>(info, work, 1);
    }

    Napi::Value waitForNotifyAsync(const Napi::CallbackInfo& info) {
        auto work = [this]() -> void {
            subscriber.waitForNotify();
        };
        return createAsyncMethod<void>(info, work, 0);
    }

    Napi::Value waitForNotifyMSAsync(const Napi::CallbackInfo& info) {
        long long timeout = info[0].As<Napi::Number>().Int64Value();
        auto work = [this, timeout]() -> void {
            subscriber.waitForNotify(chrono::milliseconds(timeout));
        };
        return createAsyncMethod<void>(info, work, 1);
    }

    Subscriber<T> subscriber;
};

// Macro to register base types with atomics
#define DECLARE_WITH_ATOMIC(TYPE, NAME) \
    PublisherWrapper<TYPE>::Init(env, exports, "Publisher_" NAME); \
    SubscriberWrapper<TYPE>::Init(env, exports, "Subscriber_" NAME); \
    PublisherWrapper<atomic<TYPE>>::Init(env, exports, "Publisher_atomic_" NAME); \
    SubscriberWrapper<atomic<TYPE>>::Init(env, exports, "Subscriber_atomic_" NAME);

// Macro to register types
#define DECLARE(TYPE, NAME) \
    PublisherWrapper<TYPE>::Init(env, exports, "Publisher_" NAME); \
    SubscriberWrapper<TYPE>::Init(env, exports, "Subscriber_" NAME);

// Declares everything
Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    DECLARE_WITH_ATOMIC(bool, "bool")
    DECLARE_WITH_ATOMIC(int, "int")
    DECLARE_WITH_ATOMIC(uint, "uint")
    DECLARE_WITH_ATOMIC(int8_t, "int8")
    DECLARE_WITH_ATOMIC(uint8_t, "uint8")
    DECLARE_WITH_ATOMIC(int16_t, "int16")
    DECLARE_WITH_ATOMIC(uint16_t, "uint16")
    DECLARE_WITH_ATOMIC(int64_t, "int64")
    DECLARE_WITH_ATOMIC(uint64_t, "uint64")
    DECLARE_WITH_ATOMIC(float, "float")
    DECLARE_WITH_ATOMIC(double, "double")
    
    // Custom types
    DECLARE(FixedString<2048>, "FixedString2048")
    DECLARE(ExampleClass, "ExampleClass")
    DECLARE(ExampleClassAtomic, "ExampleClassAtomic")
    
    return exports;
}

// Utility function to convert JS types to C++ types
template<typename T>
remove_atomic_t<T> convertFromJS(const Napi::Value& value) {
    
    if constexpr (is_same_v<remove_atomic_t<T>, bool>) {
        return value.As<Napi::Boolean>().Value();
    }
    else if constexpr (is_integral_v<remove_atomic_t<T>>) {
        return static_cast<remove_atomic_t<T>>(value.As<Napi::Number>().Int64Value());
    }
    else if constexpr (is_floating_point_v<remove_atomic_t<T>>) {
        return static_cast<remove_atomic_t<T>>(value.As<Napi::Number>().DoubleValue());
    }
    else if constexpr (is_same_v<T, FixedString<2048>>) {
        string str = value.As<Napi::String>().Utf8Value();
        return T(str.c_str());
    }
    else if constexpr (is_same_v<T, ExampleClass>) {
        Napi::Object obj = value.As<Napi::Object>();
        T result;
        if (obj.Has("value1")) result.value1 = obj.Get("value1").As<Napi::Number>().Int32Value();
        if (obj.Has("value2")) result.value2 = obj.Get("value2").As<Napi::Number>().DoubleValue();
        if (obj.Has("value3")) result.value3 = static_cast<long>(obj.Get("value3").As<Napi::Number>().Int64Value());
        return result;
    }
    else if constexpr (is_same_v<T, ExampleClassAtomic>) {
        Napi::Object obj = value.As<Napi::Object>();
        T result;
        auto getValue1 = obj.Get("getValue1").As<Napi::Function>();
        auto getValue2 = obj.Get("getValue2").As<Napi::Function>();
        auto getValue3 = obj.Get("getValue3").As<Napi::Function>();
        
        result.value1 = getValue1.Call(obj, {}).As<Napi::Number>().Int32Value();
        result.value2 = getValue2.Call(obj, {}).As<Napi::Number>().DoubleValue();
        result.value3 = static_cast<long>(getValue3.Call(obj, {}).As<Napi::Number>().Int64Value());
        return result;
    }
    return remove_atomic_t<T>{};
}

// Utility function to convert C++ types to JS types
template<typename T>
Napi::Value convertToJS(Napi::Env env, T& value) {
    if constexpr (is_std_atomic<T>::value) {
        return convertToJS(env, value.load());
    }
    else if constexpr (is_same_v<T, bool>) {
        return Napi::Boolean::New(env, value);
    }
    else if constexpr (is_integral_v<T>) {
        return Napi::Number::New(env, static_cast<double>(value));
    }
    else if constexpr (is_floating_point_v<T>) {
        return Napi::Number::New(env, static_cast<double>(value));
    }
    else if constexpr (is_same_v<T, FixedString<2048>>) {
        return Napi::String::New(env, value.get());
    }
    else if constexpr (is_same_v<T, ExampleClass>) {
        auto pValue = make_shared<T>(move(value));
        Napi::Object obj = Napi::Object::New(env);
        
        // Store the shared_ptr in the object's internal field for lifetime management
        obj.Set("_internal", Napi::External<shared_ptr<T>>::New(env, new shared_ptr<T>(pValue)));
        
        // Define properties (variables) that directly access the C++ object
        obj.DefineProperty(Napi::PropertyDescriptor::Accessor(
            // Name
            "value1",
            // Getter
            [](const Napi::CallbackInfo& info) {
                auto ptr = info.This().As<Napi::Object>().Get("_internal").As<Napi::External<shared_ptr<T>>>().Data();
                return Napi::Number::New(info.Env(), (*ptr)->value1);
            },
            // Setter
            [](const Napi::CallbackInfo& info) {
                auto ptr = info.This().As<Napi::Object>().Get("_internal").As<Napi::External<shared_ptr<T>>>().Data();
                (*ptr)->value1 = info[0].As<Napi::Number>().Int32Value();
            }
        ));
        
        obj.DefineProperty(Napi::PropertyDescriptor::Accessor(
            "value2",
            [](const Napi::CallbackInfo& info) {
                auto ptr = info.This().As<Napi::Object>().Get("_internal").As<Napi::External<shared_ptr<T>>>().Data();
                return Napi::Number::New(info.Env(), (*ptr)->value2);
            },
            [](const Napi::CallbackInfo& info) {
                auto ptr = info.This().As<Napi::Object>().Get("_internal").As<Napi::External<shared_ptr<T>>>().Data();
                (*ptr)->value2 = info[0].As<Napi::Number>().DoubleValue();
            }
        ));
        
        obj.DefineProperty(Napi::PropertyDescriptor::Accessor(
            "value3",
            [](const Napi::CallbackInfo& info) {
                auto ptr = info.This().As<Napi::Object>().Get("_internal").As<Napi::External<shared_ptr<T>>>().Data();
                return Napi::Number::New(info.Env(), (*ptr)->value3);
            },
            [](const Napi::CallbackInfo& info) {
                auto ptr = info.This().As<Napi::Object>().Get("_internal").As<Napi::External<shared_ptr<T>>>().Data();
                (*ptr)->value3 = static_cast<long>(info[0].As<Napi::Number>().Int64Value());
            }
        ));
        
        obj.Set("printValues", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
            auto ptr = info.This().As<Napi::Object>().Get("_internal").As<Napi::External<shared_ptr<T>>>().Data();
            (*ptr)->printValues();
            return info.Env().Undefined();
        }));
        return obj;
    }
    else if constexpr (is_same_v<T, ExampleClassAtomic>) {
        auto pValue = make_shared<T>(value);
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("printValues", Napi::Function::New(env, [pValue](const Napi::CallbackInfo& info) {
            pValue->printValues();
            return info.Env().Undefined();
        }));

        // Here we could use the ::Accessor, but let's be consistent with other languages
        obj.Set("getValue1", Napi::Function::New(env, [pValue](const Napi::CallbackInfo& info) {
            return Napi::Number::New(info.Env(), pValue->getValue1());
        }));
        
        obj.Set("getValue2", Napi::Function::New(env, [pValue](const Napi::CallbackInfo& info) {
            return Napi::Number::New(info.Env(), pValue->getValue2());
        }));
        
        obj.Set("getValue3", Napi::Function::New(env, [pValue](const Napi::CallbackInfo& info) {
            return Napi::Number::New(info.Env(), pValue->getValue3());
        }));
    
        obj.Set("setValue1", Napi::Function::New(env, [pValue](const Napi::CallbackInfo& info) {
            pValue->setValue1(info[0].As<Napi::Number>().Int32Value());
            return info.Env().Undefined();
        }));
        
        obj.Set("setValue2", Napi::Function::New(env, [pValue](const Napi::CallbackInfo& info) {
            pValue->setValue2(info[0].As<Napi::Number>().DoubleValue());
            return info.Env().Undefined();
        }));
        
        obj.Set("setValue3", Napi::Function::New(env, [pValue](const Napi::CallbackInfo& info) {
            pValue->setValue3(static_cast<long>(info[0].As<Napi::Number>().Int64Value()));
            return info.Env().Undefined();
        }));
        return obj;
    }
    return env.Null();
}


template<typename ReturnType>
class AsyncWorker : public Napi::AsyncWorker {
public:
    using WorkFunction = function<ReturnType()>;
    
    AsyncWorker(Napi::Function& callback, WorkFunction work)
        : Napi::AsyncWorker(callback), workFunction(work) {}

    void Execute() override {
        if constexpr (is_same_v<ReturnType, void>) {
            workFunction();
        }
        else{
            result = workFunction();
        }
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        // Return type is either void or optional<>
        if constexpr (is_same_v<ReturnType, void>) {
            Callback().Call({Env().Null()});
        } 
        else{
            if(result.has_value()){
                Callback().Call({Env().Null(), convertToJS(Env(), result.value())});
            }
            else{
                Callback().Call({Env().Null(), Env().Null()});
            }
        }
    }

private:
    WorkFunction workFunction;
    typename conditional<is_same_v<ReturnType, void>, int, ReturnType>::type result;
};

// Helper to create async methods
template<typename ReturnType>
Napi::Value createAsyncMethod(const Napi::CallbackInfo& info, 
                                typename AsyncWorker<ReturnType>::WorkFunction work,
                                int callbackIndex) {
    if(callbackIndex >= 0 && info.Length() > callbackIndex && info[callbackIndex].IsFunction()){
        // Callback-based async
        Napi::Function callback = info[callbackIndex].As<Napi::Function>();
        auto* worker = new AsyncWorker<ReturnType>(callback, work);
        worker->Queue();
        return info.Env().Undefined();
    }
    else{
        // Promise-based async
        auto deferred = Napi::Promise::Deferred::New(info.Env());
        auto callback = Napi::Function::New(info.Env(), [deferred](const Napi::CallbackInfo& cbInfo) {
            if (cbInfo[0].IsNull()) {
                if (cbInfo.Length() > 1) {
                    deferred.Resolve(cbInfo[1]);
                }
                else{
                    deferred.Resolve(cbInfo.Env().Undefined());
                }
            }
            else{
                deferred.Reject(cbInfo[0]);
            }
            return cbInfo.Env().Undefined();
        });
        
        auto* worker = new AsyncWorker<ReturnType>(callback, work);
        worker->Queue();
        return deferred.Promise();
    }
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)