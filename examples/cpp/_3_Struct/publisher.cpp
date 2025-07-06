#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "Examples.h"
using namespace std;

int main(){
    
    shps::Publisher<ExampleStruct> publisher("Struct");
    ExampleStruct exampleStruct;
    while(1){
        /*--------- Example 1 : Publish  ---------*/
        exampleStruct = publisher.readValue();
        exampleStruct.value1+=1;
        exampleStruct.value2+=0.1;
        exampleStruct.value3+=10;
        publisher.publish(exampleStruct);
        cout << "PUBLISHER : " << dec << 
        exampleStruct.value1 << ", " <<
        exampleStruct.value2 << ", " <<
        exampleStruct.value3 << "" <<
        endl;
        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        ExampleStruct* rawExampleStruct = publisher.rawValue();
        rawExampleStruct->value1+=1;
        rawExampleStruct->value2+=0.1;
        rawExampleStruct->value3+=10;
        publisher.notifyAll();

        cout << "PUBLISHER : " << dec << 
        rawExampleStruct->value1 << ", " <<
        rawExampleStruct->value2 << ", " <<
        rawExampleStruct->value3 << " RAW VALUE" <<
        endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}