#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "Examples.h"
using namespace std;

int main(){
    
    shps::Publisher<ExampleClassAtomic> publisher("ObjectAtomic");
    ExampleClassAtomic exampleObjectAtomic;
    while(1){
        /*--------- Example 1 : Publish  ---------*/
        exampleObjectAtomic = publisher.readValue();
        exampleObjectAtomic.value1+=1;
        exampleObjectAtomic.value2+=0.1;
        exampleObjectAtomic.value3+=10;
        publisher.publish(exampleObjectAtomic);
        exampleObjectAtomic.printValues();

        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        ExampleClassAtomic* rawExampleObjectAtomic = publisher.rawValue();
        rawExampleObjectAtomic->value1+=1;
        rawExampleObjectAtomic->value2+=0.1;
        rawExampleObjectAtomic->value3+=10;
        publisher.notifyAll();
        rawExampleObjectAtomic->printValues();
        this_thread::sleep_for(1s);
    }
    return 0;
}