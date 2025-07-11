#include <napi.h>
#include "SharedPubSub.hpp"
#include "FixedString.h"
#include "Examples.h"
#include <chrono>

using namespace shps;
using namespace std;

// Forward declaration of utility function
template<typename T>
remove_atomic_t<T> convertFromJS(const Napi::Value& value);

template<typename T>
Napi::Value convertToJS(Napi::Env env, const T& value);

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
            PublisherWrapper<T>::InstanceMethod("rawValue", &PublisherWrapper<T>::rawValue),
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

    Napi::Value rawValue(const Napi::CallbackInfo& info) {
        T* rawPtr = publisher.rawValue();
        return convertToJS(info.Env(), *rawPtr);
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
            SubscriberWrapper<T>::InstanceMethod("rawValue", &SubscriberWrapper<T>::rawValue),
            SubscriberWrapper<T>::InstanceMethod("readValue", &SubscriberWrapper<T>::readValue),
            SubscriberWrapper<T>::InstanceMethod("readWait", &SubscriberWrapper<T>::readWait),
            SubscriberWrapper<T>::InstanceMethod("readWaitMS", &SubscriberWrapper<T>::readWaitMS),
            SubscriberWrapper<T>::InstanceMethod("waitForNotify", &SubscriberWrapper<T>::waitForNotify),
            SubscriberWrapper<T>::InstanceMethod("waitForNotifyMS", &SubscriberWrapper<T>::waitForNotifyMS),
        });
        exports.Set(className, func);
        return exports;
    }

private:
    Napi::Value subscribe(const Napi::CallbackInfo& info) {
        bool result = subscriber.subscribe();
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

    Napi::Value rawValue(const Napi::CallbackInfo& info) {
        T* rawPtr = subscriber.rawValue();
        return convertToJS(info.Env(), *rawPtr);
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
    } else if constexpr (is_integral_v<remove_atomic_t<T>>) {
        return static_cast<remove_atomic_t<T>>(value.As<Napi::Number>().Int64Value());
    } else if constexpr (is_floating_point_v<remove_atomic_t<T>>) {
        return static_cast<remove_atomic_t<T>>(value.As<Napi::Number>().DoubleValue());
    } else if constexpr (is_same_v<T, FixedString<2048>>) {
        string str = value.As<Napi::String>().Utf8Value();
        return T(str.c_str());
    } else if constexpr (is_same_v<T, ExampleClass>) {
        Napi::Object obj = value.As<Napi::Object>();
        T result;
        if (obj.Has("value1")) result.value1 = obj.Get("value1").As<Napi::Number>().Int32Value();
        if (obj.Has("value2")) result.value2 = obj.Get("value2").As<Napi::Number>().DoubleValue();
        if (obj.Has("value3")) result.value3 = static_cast<long>(obj.Get("value3").As<Napi::Number>().Int64Value());
        return result;
    } else if constexpr (is_same_v<T, ExampleClassAtomic>) {
        Napi::Object obj = value.As<Napi::Object>();
        T result;
        if (obj.Has("value1")) result.value1 = obj.Get("value1").As<Napi::Number>().Int32Value();
        if (obj.Has("value2")) result.value2 = obj.Get("value2").As<Napi::Number>().DoubleValue();
        if (obj.Has("value3")) result.value3 = static_cast<long>(obj.Get("value3").As<Napi::Number>().Int64Value());
        return result;
    }
    return remove_atomic_t<T>{};
}

// Utility function to convert C++ types to JS types
template<typename T>
Napi::Value convertToJS(Napi::Env env, const T& value) {
    if constexpr (is_std_atomic<T>::value) {
        return convertToJS(env, value.load());
    } else if constexpr (is_same_v<T, bool>) {
        return Napi::Boolean::New(env, value);
    } else if constexpr (is_integral_v<T>) {
        return Napi::Number::New(env, static_cast<double>(value));
    } else if constexpr (is_floating_point_v<T>) {
        return Napi::Number::New(env, static_cast<double>(value));
    } else if constexpr (is_same_v<T, FixedString<2048>>) {
        return Napi::String::New(env, value.get());
    } else if constexpr (is_same_v<T, ExampleClass>) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("value1", Napi::Number::New(env, value.value1));
        obj.Set("value2", Napi::Number::New(env, value.value2));
        obj.Set("value3", Napi::Number::New(env, value.value3));
        return obj;
    } else if constexpr (is_same_v<T, ExampleClassAtomic>) {
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("value1", Napi::Number::New(env, value.value1.load()));
        obj.Set("value2", Napi::Number::New(env, value.value2.load()));
        obj.Set("value3", Napi::Number::New(env, value.value3.load()));
        return obj;
    }
    return env.Null();
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)