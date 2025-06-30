#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    
    shps::Publisher<atomic<int>> publisher("PubSub");
    int value = 0;
    
    while(1){
        /*--------- Method 1 : Publish ---------*/
        publisher.publish(++value);
        
        cout << "PUBLISHER : " << dec << value << " Normal publish"<< endl;
        this_thread::sleep_for(1s);

        /*--------- Method 2 : Publish only on change ---------*/
        
        for(int i=0;i<100;++i){
            publisher.publishOnChange(value);
        }
        publisher.publishOnChange(++value);

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