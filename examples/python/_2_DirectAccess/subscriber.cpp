#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){

    shps::Subscriber<atomic<int>> subscriber("DirectAccess", "DirectAccessSubscriber1");
    int value = 0;
    
    while(1){
        /*--------- Example 1 : Read Value  ---------*/
        value = subscriber.readValue();
        cout << "SUBSCRIBER : " << dec << value  << " readValue()" << endl;
        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get a pointer to the value and read it  ---------*/
        atomic<int>* rawValue = subscriber.rawValue();
        cout << "SUBSCRIBER : " << dec << rawValue->load() << " Direct access" << endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}