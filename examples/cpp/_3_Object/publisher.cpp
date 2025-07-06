#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "Examples.h"
using namespace std;

int main(){
    
    shps::Publisher<ExampleClass> publisher("Object");
    ExampleClass exampleObject;
    while(1){
        /*--------- Example 1 : Publish  ---------*/
        exampleObject = publisher.readValue();
        exampleObject.value1+=1;
        exampleObject.value2+=0.1;
        exampleObject.value3+=10;
        publisher.publish(exampleObject);
        exampleObject.printValues();

        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        ExampleClass* rawExampleObject = publisher.rawValue();
        rawExampleObject->value1+=1;
        rawExampleObject->value2+=0.1;
        rawExampleObject->value3+=10;
        publisher.notifyAll();
        rawExampleObject->printValues();
        this_thread::sleep_for(1s);
    }
    return 0;
}