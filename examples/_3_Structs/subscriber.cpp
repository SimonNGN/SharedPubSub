#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

struct CustomStruct{
    int value1 = 0;
    float value2 = 0.0;
    long int value3 = 42;
};

int main(){

    shps::Subscriber<CustomStruct> subscriber("Struct", "StructSubscriber1",true);
    
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
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value (not thread safe here)---------*/
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