#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    // Create the publisher
    // Raw value is atomic and thread safe
    shps::Publisher<atomic<int>> publisher("DirectAccess");

    // Value that we will copy to the publisher's data
    int value = 0;
    
    while(1){
        // Increment the value
        value++;
        // Publish. Will set the value, push to subscribers' queue and notify
        publisher.setValue(value);
        cout << "PUBLISHER : " << dec << value << endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}