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
        /*--------- Method 1 : Set Value  ---------*/
        publisher.setValue(++value);
        cout << "PUBLISHER : " << dec << value << " setValue()"<< endl;
        this_thread::sleep_for(1s);

        /*--------- Method 2 : Get a pointer to the value and change it  ---------*/
        atomic<int>* rawValue = publisher.rawValue();
        rawValue->store(++value);
        cout << "PUBLISHER : " << dec << publisher.readValue() << " Direct access"<< endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}