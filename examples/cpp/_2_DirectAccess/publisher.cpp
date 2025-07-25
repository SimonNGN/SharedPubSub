#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    
    shps::Publisher<atomic<int>> publisher("DirectAccess");
    int value = 0;
    
    while(1){
        /*--------- Example 1 : Set Value  ---------*/
        publisher.setValue(++value);
        cout << "PUBLISHER C++: " << dec << value << " setValue()"<< endl;
        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get a pointer to the value and change it  ---------*/
        atomic<int>* rawValue = publisher.rawValue();
        rawValue->store(++value);
        cout << "PUBLISHER C++: " << dec << publisher.readValue() << " Direct access"<< endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}