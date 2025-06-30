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
    
    shps::Publisher<CustomStruct> publisher("StructAtomic");
    CustomStruct customStruct;
    while(1){
        /*--------- Example 1 : Publish  ---------*/
        customStruct = publisher.readValue();
        customStruct.value1+=1;
        customStruct.value2+=0.1;
        customStruct.value3+=10;
        publisher.publish(customStruct);
        cout << "PUBLISHER : " << dec << 
        customStruct.value1 << ", " <<
        customStruct.value2 << ", " <<
        customStruct.value3 << "" <<
        endl;
        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        CustomStruct* rawCustomStruct = publisher.rawValue();
        rawCustomStruct->value1+=1;
        rawCustomStruct->value2+=0.1;
        rawCustomStruct->value3+=10;
        publisher.notifyAll();

        cout << "PUBLISHER : " << dec << 
        rawCustomStruct->value1 << ", " <<
        rawCustomStruct->value2 << ", " <<
        rawCustomStruct->value3 << " RAW VALUE" <<
        endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}