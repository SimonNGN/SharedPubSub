#include <napi.h>
#include "SharedPubSub.hpp"
#include "FixedString.h"
#include "Examples.h"
#include <chrono>

using namespace shps;
using namespace std;

// Template helper for type conversion
template<typename T>
remove_atomic_t<T> convertFromJS(const Napi::Value& value) {
    if constexpr (is_same_v<T, bool>) {
        return value.As<Napi::Boolean>().Value();
    } else if constexpr (is_integral_v<T>) {
        return static_cast<T>(value.As<Napi::Number>().Int64Value());
    } else if constexpr (is_floating_point_v<T>) {
        return static_cast<T>(value.As<Napi::Number>().DoubleValue());
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
    return T{};
}

template<typename T>
Napi::Value convertToJS(Napi::Env env, const T& value) {
    if constexpr (is_same_v<T, bool>) {
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

// Simplified wrapper classes
template<typename T>
class PublisherWrapper : public Napi::ObjectWrap<PublisherWrapper<T>> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports, const string& className) {
        Napi::Function func = PublisherWrapper<T>::DefineClass(env, className.c_str(), {
            PublisherWrapper<T>::InstanceMethod("publish", &PublisherWrapper<T>::Publish),
            PublisherWrapper<T>::InstanceMethod("publishOnChange", &PublisherWrapper<T>::PublishOnChange),
            PublisherWrapper<T>::InstanceMethod("setValue", &PublisherWrapper<T>::SetValue),
            PublisherWrapper<T>::InstanceMethod("setValueAndNotifyOnChange", &PublisherWrapper<T>::SetValueAndNotifyOnChange),
            PublisherWrapper<T>::InstanceMethod("readValue", &PublisherWrapper<T>::ReadValue),
            PublisherWrapper<T>::InstanceMethod("notifyAll", &PublisherWrapper<T>::NotifyAll),
        });
        exports.Set(className, func);
        return exports;
    }

    PublisherWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<PublisherWrapper<T>>(info) {
        string topicName = info[0].As<Napi::String>().Utf8Value();
        publisher_ = new Publisher<T>(topicName);
    }

private:
    Napi::Value Publish(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher_->publish(value);
        return info.Env().Undefined();
    }

    Napi::Value PublishOnChange(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher_->publishOnChange(value);
        return info.Env().Undefined();
    }

    Napi::Value SetValue(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher_->setValue(value);
        return info.Env().Undefined();
    }

    Napi::Value SetValueAndNotifyOnChange(const Napi::CallbackInfo& info) {
        remove_atomic_t<T> value = convertFromJS<T>(info[0]);
        publisher_->setValueAndNotifyOnChange(value);
        return info.Env().Undefined();
    }

    Napi::Value ReadValue(const Napi::CallbackInfo& info) {
        auto value = publisher_->readValue();
        return convertToJS(info.Env(), value);
    }

    Napi::Value NotifyAll(const Napi::CallbackInfo& info) {
        publisher_->notifyAll();
        return info.Env().Undefined();
    }

    Publisher<T>* publisher_;
};

template<typename T>
class SubscriberWrapper : public Napi::ObjectWrap<SubscriberWrapper<T>> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports, const string& className) {
        Napi::Function func = SubscriberWrapper<T>::DefineClass(env, className.c_str(), {
            SubscriberWrapper<T>::InstanceMethod("subscribe", &SubscriberWrapper<T>::Subscribe),
            SubscriberWrapper<T>::InstanceMethod("readValue", &SubscriberWrapper<T>::ReadValue),
            SubscriberWrapper<T>::InstanceMethod("readWait", &SubscriberWrapper<T>::ReadWait),
            SubscriberWrapper<T>::InstanceMethod("readWaitMS", &SubscriberWrapper<T>::ReadWaitMS),
            SubscriberWrapper<T>::InstanceMethod("waitForNotify", &SubscriberWrapper<T>::WaitForNotify),
            SubscriberWrapper<T>::InstanceMethod("waitForNotifyMS", &SubscriberWrapper<T>::WaitForNotifyMS),
            SubscriberWrapper<T>::InstanceMethod("rawValue", &SubscriberWrapper<T>::RawValue),
        });
        exports.Set(className, func);
        return exports;
    }

    SubscriberWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<SubscriberWrapper<T>>(info) {
        string topicName = info[0].As<Napi::String>().Utf8Value();
        string subscriberName = info[1].As<Napi::String>().Utf8Value();
        bool doSubscribe = info.Length() > 2 ? info[2].As<Napi::Boolean>().Value() : false;
        subscriber_ = new Subscriber<T>(topicName, subscriberName, doSubscribe);
    }

private:
    Napi::Value Subscribe(const Napi::CallbackInfo& info) {
        bool result = subscriber_->subscribe();
        return Napi::Boolean::New(info.Env(), result);
    }

    Napi::Value ReadValue(const Napi::CallbackInfo& info) {
        auto value = subscriber_->readValue();
        return convertToJS(info.Env(), value);
    }

    Napi::Value ReadWait(const Napi::CallbackInfo& info) {
        auto result = subscriber_->readWait();
        if (!result.has_value()) return info.Env().Null();
        return convertToJS(info.Env(), result.value());
    }

    Napi::Value ReadWaitMS(const Napi::CallbackInfo& info) {
        int timeout = info[0].As<Napi::Number>().Int32Value();
        auto result = subscriber_->readWait(chrono::milliseconds(timeout));
        if (!result.has_value()) return info.Env().Null();
        return convertToJS(info.Env(), result.value());
    }

    Napi::Value WaitForNotify(const Napi::CallbackInfo& info) {
        subscriber_->waitForNotify();
        return info.Env().Undefined();
    }

    Napi::Value WaitForNotifyMS(const Napi::CallbackInfo& info) {
        int timeout = info[0].As<Napi::Number>().Int32Value();
        subscriber_->waitForNotify(chrono::milliseconds(timeout));
        return info.Env().Undefined();
    }

    Napi::Value RawValue(const Napi::CallbackInfo& info) {
        T* rawPtr = subscriber_->rawValue();
        return convertToJS(info.Env(), *rawPtr);
    }

    Subscriber<T>* subscriber_;
};

// Macro to register types
#define REGISTER_TYPE(TYPE, NAME) \
    PublisherWrapper<TYPE>::Init(env, exports, "Publisher_" NAME); \
    SubscriberWrapper<TYPE>::Init(env, exports, "Subscriber_" NAME);

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    REGISTER_TYPE(bool, "bool")
    REGISTER_TYPE(int, "int")
    REGISTER_TYPE(unsigned int, "uint")
    REGISTER_TYPE(int8_t, "int8")
    REGISTER_TYPE(uint8_t, "uint8")
    REGISTER_TYPE(int16_t, "int16")
    REGISTER_TYPE(uint16_t, "uint16")
    REGISTER_TYPE(int64_t, "int64")
    REGISTER_TYPE(uint64_t, "uint64")
    REGISTER_TYPE(float, "float")
    REGISTER_TYPE(double, "double")
    
    // Custom types - using correct type names
    REGISTER_TYPE(FixedString<2048>, "FixedString2048")
    REGISTER_TYPE(ExampleClass, "ExampleClass")
    REGISTER_TYPE(ExampleClassAtomic, "ExampleClassAtomic")
    
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)