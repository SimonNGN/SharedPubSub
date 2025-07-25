#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    shps::Subscriber<int> subscriber("PubSub", "PubSubSubscriberCpp",true);
    optional<int> value = nullopt;
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/

        value = subscriber.readWait();
        // Verify if the queue had a value on notification.
        if(value.has_value()){
            cout << "SUBSCRIBER C++: " << dec << value.value() << endl;
        }
        else{
            cout << "SUBSCRIBER C++: No value in queue" << endl;
        }

        /*--------- Example 2 : Wait for notification with timeout ---------*/
        value = subscriber.readWait(2s);
        // Verify if the queue had a value on notification.
        if(value.has_value()){
            cout << "SUBSCRIBER C++: " << dec << value.value() << endl;
        }
        else{
            cout << "SUBSCRIBER C++: No value in queue" << endl;
        }
    }
    return 0;
}