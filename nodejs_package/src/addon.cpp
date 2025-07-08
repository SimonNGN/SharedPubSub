#include <napi.h>               // Node-API header
#include "../plugin/SharedPubSub.h" // Include your header-only library

// N-API wrapper for MyPlugin::calculate_sum
Napi::Value CalculateSumWrapper(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  // Basic argument validation
  if (info.Length() < 3 || !info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsNumber()) {
    Napi::TypeError::New(env, "Three numbers expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  // Get arguments and call your C++ function
  int a = info[0].As<Napi::Number>().Int32Value();
  int b = info[1].As<Napi::Number>().Int32Value();
  int c = info[2].As<Napi::Number>().Int32Value();

  int result = MyPlugin::calculate_sum(a, b, c);

  // Return the result to JavaScript
  return Napi::Number::New(env, result);
}

// N-API wrapper for MyPlugin::calculate_vector_sum
Napi::Value CalculateVectorSumWrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsArray()) {
        Napi::TypeError::New(env, "Array expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Array jsArray = info[0].As<Napi::Array>();
    std::vector<int> cppVector; // Assuming integer vector for simplicity

    for (uint32_t i = 0; i < jsArray.Length(); ++i) {
        Napi::Value val = jsArray[i];
        if (!val.IsNumber()) {
            Napi::TypeError::New(env, "Array must contain only numbers").ThrowAsJavaScriptException();
            return env.Null();
        }
        cppVector.push_back(val.As<Napi::Number>().Int32Value());
    }

    int sum = MyPlugin::calculate_vector_sum(cppVector);
    return Napi::Number::New(env, sum);
}


// N-API wrapper for MyPlugin::Greeter class
// This demonstrates exposing a C++ class with methods
class GreeterWrapper : public Napi::ObjectWrap<GreeterWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    GreeterWrapper(const Napi::CallbackInfo& info);

private:
    Napi::Value GetGreeting(const Napi::CallbackInfo& info);
    MyPlugin::Greeter* _greeter; // Pointer to our C++ Greeter object
};

Napi::Object GreeterWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Greeter", {
        InstanceMethod("getGreeting", &GreeterWrapper::GetGreeting),
        // Add more instance methods here if your Greeter class has them
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("Greeter", func);
    return exports;
}

GreeterWrapper::GreeterWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<GreeterWrapper>(info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected for Greeter name").ThrowAsJavaScriptException();
        return;
    }
    std::string name = info[0].As<Napi::String>().Utf8Value();
    _greeter = new MyPlugin::Greeter(name); // Create instance of your C++ class
}

Napi::Value GreeterWrapper::GetGreeting(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), _greeter->get_greeting());
}


// Main initialization function for the addon
Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "calculateSum"), Napi::Function::New(env, CalculateSumWrapper));
  exports.Set(Napi::String::New(env, "calculateVectorSum"), Napi::Function::New(env, CalculateVectorSumWrapper));
  GreeterWrapper::Init(env, exports); // Initialize the Greeter class
  return exports;
}

// Register the addon module with Node.js
NODE_API_MODULE(NODE_GYP_ADDON_NAME, InitAll) // NODE_GYP_ADDON_NAME is a macro from node-gyp/cmake-js for the target name