#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

class CustomClass{
    public :
        int value1 = 0;
        float value2 = 0.0;
        long int value3 = 42;

        void printValues(){
            cout << "CUSTOM CLASS : " << dec << 
            value1 << ", " <<
            value2 << ", " <<
            value3 << "" <<
            endl;
        };

        CustomClass() = default;
        ~CustomClass() = default;
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
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value (not thread safe if not atomic) ---------*/
            CustomClass customClass2 = subscriber.readValue();
            customClass2.printValues();
        }
    }
    return 0;
}