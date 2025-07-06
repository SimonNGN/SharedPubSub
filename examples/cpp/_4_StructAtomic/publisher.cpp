#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "Examples.h"
using namespace std;

// To make atomic variable works in structs or objects,
// You need to make a custom copy constructor and copy assigment
// And a default constructor

int main(){
    
    shps::Publisher<ExampleStructAtomic> publisher("StructAtomic");
    ExampleStructAtomic exampleStructAtomic;
    while(1){
        /*--------- Example 1 : Publish  ---------*/
        exampleStructAtomic = publisher.readValue();
        exampleStructAtomic.value1+=1;
        exampleStructAtomic.value2+=0.1;
        exampleStructAtomic.value3+=10;
        publisher.publish(exampleStructAtomic);
        cout << "PUBLISHER : " << dec << 
        exampleStructAtomic.value1 << ", " <<
        exampleStructAtomic.value2 << ", " <<
        exampleStructAtomic.value3 << "" <<
        endl;
        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        ExampleStructAtomic* rawExampleStructAtomic = publisher.rawValue();
        rawExampleStructAtomic->value1+=1;
        rawExampleStructAtomic->value2+=0.1;
        rawExampleStructAtomic->value3+=10;
        publisher.notifyAll();

        cout << "PUBLISHER : " << dec << 
        rawExampleStructAtomic->value1 << ", " <<
        rawExampleStructAtomic->value2 << ", " <<
        rawExampleStructAtomic->value3 << " RAW VALUE" <<
        endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}