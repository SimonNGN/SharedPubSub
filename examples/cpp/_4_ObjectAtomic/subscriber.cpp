#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "Examples.h"
using namespace std;

int main(){

    shps::Subscriber<ExampleClassAtomic> subscriber("Object", "ObjectSubscriber1",true);
    
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        optional<ExampleClassAtomic> exampleObjectAtomic1 = subscriber.readWait();
        if(exampleObjectAtomic1.has_value()){
            exampleObjectAtomic1.value().printValues();
        }
        else{
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value ---------*/
            // Thread safe because all members are atomic
            ExampleClassAtomic* exampleObjectAtomic1 = subscriber.rawValue();
            exampleObjectAtomic1->printValues();
        }
    }
    return 0;
}