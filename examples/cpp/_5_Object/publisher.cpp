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