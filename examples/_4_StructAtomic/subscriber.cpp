#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

// To make atomic variable works in structs or objects,
// You need to make a custom copy constructor and copy assigment
// And a default constructor
struct CustomStruct{
    atomic<int> value1 = 0;
    atomic<float> value2 = 0.0;
    atomic<long> value3 = 42;

    // Default constructor
    CustomStruct() = default;

    // Copy constructor
    CustomStruct(const CustomStruct& other):
        value1(other.value1.load()), // Atomic uses load() to get the value
        value2(other.value2.load()), 
        value3(other.value3.load()){}

    // Copy assignment
    CustomStruct& operator=(const CustomStruct& other) {
        if (this != &other) {
            value1 = other.value1.load();
            value2 = other.value2.load();
            value3 = other.value3.load();
        }
        return *this;
    }
};

int main(){

    shps::Subscriber<CustomStruct> subscriber("StructAtomic", "StructAtomicSubscriber1",true);
    
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        optional<CustomStruct> customStruct1 = subscriber.readWait();
        if(customStruct1.has_value()){
            cout << "SUBSCRIBER : " << dec << 
            customStruct1.value().value1 << ", " <<
            customStruct1.value().value2 << ", " <<
            customStruct1.value().value3 << " " <<
            endl;
        }
        else{
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value (thread safe)---------*/
            CustomStruct customStruct2 = subscriber.readValue();
            cout << "SUBSCRIBER : " << dec << 
            customStruct2.value1 << ", " <<
            customStruct2.value2 << ", " <<
            customStruct2.value3 << " CURRENT VALUE" <<
            endl;
        }
    }
    return 0;
}