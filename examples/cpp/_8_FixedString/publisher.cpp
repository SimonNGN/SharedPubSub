#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
#include "FixedString.h"
using namespace std;

int main(){
    
    shps::Publisher<FixedString<2048>> publisher("FixedString");
    FixedString<2048> value;
    
    while(1){
        /*--------- Example 1 : Publish ---------*/
        value = "EXAMPLE 1";
        publisher.publish(value);
        
        cout << "PUBLISHER : " << value.get() << " Normal publish"<< endl;
        this_thread::sleep_for(1s);

        /*--------- Example 2 : Publish only on change ---------*/
        
        for(int i=0;i<100;++i){
            publisher.publishOnChange(value);
        }
        value = "EXAMPLE 2";
        publisher.publishOnChange(value);

        cout << "PUBLISHER : " << value.get() << " Published on change" << endl;
        this_thread::sleep_for(1s);

        /*--------- Example 3 : Push multiple in queue and notify after ---------*/
        
        for(int i=0;i<10;++i){
            value = "EXAMPLE 3 "+to_string(i);
            publisher.setValueAndPush(value);
            cout << "PUBLISHER : " << value.get() << endl;
        }
        publisher.notifyAll();
        this_thread::sleep_for(1s);
    }
    return 0;
}