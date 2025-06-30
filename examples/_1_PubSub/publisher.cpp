#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    // Create the publisher
    // Raw value is atomic and thread safe
    shps::Publisher<atomic<int>> publisher("PubSub");

    // Value that we will copy to the publisher's data
    int value = 0;
    
    while(1){
        /*--------- Method 1 : Publish ---------*/

        // Increment the value
        value++;
        // Publish. Will set the value, push to subscribers' queue and notify
        publisher.publish(value);
        cout << "PUBLISHER : " << dec << value << " Normal publish"<< endl;
        this_thread::sleep_for(1s);

        /*--------- Method 2 : Publish only on change ---------*/
        
        for(int i=0;i<100;++i){
            publisher.publishOnChange(value);
        }
        publisher.publish(++value);

        cout << "PUBLISHER : " << dec << value << " Published on change" << endl;

        this_thread::sleep_for(1s);

        /*--------- Method 3 : Push multiple in queue and notify after ---------*/
        
        for(int i=0;i<10;++i){
            publisher.setValueAndPush(++value);
            cout << "PUBLISHER : " << dec << value << endl;
        }
        publisher.notifyAll();

        cout << "PUBLISHER : " << dec << value << " Sent multiple"<< endl;

        this_thread::sleep_for(1s);
    }
    return 0;
}