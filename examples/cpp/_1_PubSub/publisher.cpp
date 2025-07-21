#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    
    shps::Publisher<int> publisher("PubSub");
    int value = 0;
    
    while(1){
        /*--------- Example 1 : Publish ---------*/
        publisher.publish(++value);
        
        cout << "PUBLISHER C++ : " << dec << value << " Normal publish"<< endl;
        this_thread::sleep_for(1s);

        /*--------- Example 2 : Publish only on change ---------*/
        
        for(int i=0;i<100;++i){
            publisher.publishOnChange(value);
        }
        publisher.publishOnChange(++value);

        cout << "PUBLISHER C++ : " << dec << value << " Published on change" << endl;
        this_thread::sleep_for(1s);

        /*--------- Example 3 : Push multiple in queue and notify after ---------*/
        
        for(int i=0;i<10;++i){
            publisher.setValueAndPush(++value);
            cout << "PUBLISHER C++ : " << dec << value << endl;
        }
        publisher.notifyAll();

        cout << "PUBLISHER C++ : " << dec << value << " Sent multiple"<< endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}