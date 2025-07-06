#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "Examples.h"
using namespace std;

int main(){

    shps::Subscriber<ExampleStruct> subscriber("Struct", "StructSubscriber1",true);
    
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        optional<ExampleStruct> exampleStruct1 = subscriber.readWait();
        if(exampleStruct1.has_value()){
            cout << "SUBSCRIBER : " << dec << 
            exampleStruct1.value().value1 << ", " <<
            exampleStruct1.value().value2 << ", " <<
            exampleStruct1.value().value3 << " " <<
            endl;
        }
        else{
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value (not thread safe here)---------*/
            ExampleStruct exampleStruct2 = subscriber.readValue();
            cout << "SUBSCRIBER : " << dec << 
            exampleStruct2.value1 << ", " <<
            exampleStruct2.value2 << ", " <<
            exampleStruct2.value3 << " CURRENT VALUE" <<
            endl;
        }
    }
    return 0;
}