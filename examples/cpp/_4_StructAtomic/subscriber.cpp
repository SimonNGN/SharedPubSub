#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "Examples.h"
using namespace std;

// To make atomic variable works in structs or objects,
// You need to make a custom copy constructor and copy assigment
// And a default constructor

int main(){

    shps::Subscriber<ExampleStructAtomic> subscriber("StructAtomic", "StructAtomicSubscriber1",true);
    
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        optional<ExampleStructAtomic> exampleStructAtomic1 = subscriber.readWait();
        if(exampleStructAtomic1.has_value()){
            cout << "SUBSCRIBER : " << dec << 
            exampleStructAtomic1.value().value1 << ", " <<
            exampleStructAtomic1.value().value2 << ", " <<
            exampleStructAtomic1.value().value3 << " " <<
            endl;
        }
        else{
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value ---------*/
            // Thread safe because all members are atomic
            ExampleStructAtomic* exampleStructAtomic2 = subscriber.rawValue();
            cout << "SUBSCRIBER : " << dec << 
            exampleStructAtomic2->value1 << ", " <<
            exampleStructAtomic2->value2 << ", " <<
            exampleStructAtomic2->value3 << " CURRENT VALUE" <<
            endl;
        }
    }
    return 0;
}