#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    shps::Subscriber<atomic<int>> subscriber("DirectAccess", "DirectAccessSubscriber1",true);
    int value = 0;
    while(1){
        value = subscriber.readValue();
        cout << "SUBSCRIBER : " << dec << value << endl;
        this_thread::sleep_for(500ms);
    }
    return 0;
}