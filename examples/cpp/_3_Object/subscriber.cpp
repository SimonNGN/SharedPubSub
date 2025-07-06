#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "Examples.h"
using namespace std;

int main(){

    shps::Subscriber<ExampleClass> subscriber("Object", "ObjectSubscriberCpp",true);
    
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        optional<ExampleClass> customObject1 = subscriber.readWait();
        if(customObject1.has_value()){
            customObject1.value().printValues();
        }
        else{
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value (not thread safe if not atomic) ---------*/
            ExampleClass customObject2 = subscriber.readValue();
            customObject2.printValues();
        }
    }
    return 0;
}