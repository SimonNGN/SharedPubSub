#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

class CustomClass{
    public :
        atomic<int> value1 = 0;
        atomic<float> value2 = 0.0;
        atomic<long> value3 = 42;

        void printValues(){
            cout << "CUSTOM CLASS : " << dec << 
            value1 << ", " <<
            value2 << ", " <<
            value3 << "" <<
            endl;
        };

        CustomClass() = default;
        ~CustomClass() = default;

        // Copy constructor
        CustomClass(const CustomClass& other)
            : value1(other.value1.load()),
              value2(other.value2.load()),
              value3(other.value3.load()) {}

        // Copy assignment
        CustomClass& operator=(const CustomClass& other) {
            if (this != &other) {
                value1.store(other.value1.load());
                value2.store(other.value2.load());
                value3.store(other.value3.load());
            }
            return *this;
        }
};

int main(){
    
    shps::Publisher<CustomClass> publisher("Object");
    CustomClass customClass;
    while(1){
        /*--------- Example 1 : Publish  ---------*/
        customClass = publisher.readValue();
        customClass.value1+=1;
        customClass.value2+=0.1;
        customClass.value3+=10;
        publisher.publish(customClass);
        customClass.printValues();

        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        CustomClass* rawCustomClass = publisher.rawValue();
        rawCustomClass->value1+=1;
        rawCustomClass->value2+=0.1;
        rawCustomClass->value3+=10;
        publisher.notifyAll();
        rawCustomClass->printValues();
        this_thread::sleep_for(1s);
    }
    return 0;
}