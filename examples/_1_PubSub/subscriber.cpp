#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    shps::Subscriber<atomic<int>> subscriber("PubSub", "PubSubSubscriber1",true);
    optional<int> value = nullopt;
    while(1){
        value = subscriber.readWait();
        // Verify if the queue had a value on notification.
        if(value.has_value()){
            cout << "SUBSCRIBER : " << dec << value.value() << endl;
        }
        else{
            cout << "SUBSCRIBER : No value in queue" << endl;
        }
    }
    return 0;
}