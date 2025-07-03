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

    shps::Subscriber<CustomClass> subscriber("Object", "ObjectSubscriber1",true);
    
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        optional<CustomClass> customClass1 = subscriber.readWait();
        if(customClass1.has_value()){
            customClass1.value().printValues();
        }
        else{
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value ---------*/
            // Thread safe because all members are atomic
            CustomClass* customClass2 = subscriber.rawValue();
            customClass2->printValues();
        }
    }
    return 0;
}