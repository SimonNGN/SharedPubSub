#include <iostream>
#include <thread>
#include <cstdlib>
#include "SharedPubSub.hpp"
using namespace std;

int main(){
    
    shps::Publisher<int> publisher("Demo");

    while(1){
        system("clear");
        this_thread::sleep_for(5s);
        publisher.publish(1);
        
        cout << "PUBLISHER C++ : 1 publish()"<< endl;
        this_thread::sleep_for(1s);
        
        for(int i=0;i<5;++i){
            publisher.publishOnChange(1);
            cout << "PUBLISHER C++ : 1 publishOnChange()" << endl;
            this_thread::sleep_for(500ms);
        }
        this_thread::sleep_for(3s);
        publisher.publishOnChange(9999);

        cout << "PUBLISHER C++ : 9999 publishOnChange()" << endl;
        this_thread::sleep_for(3s);

        for(int i=0;i<=5;++i){
            publisher.setValueAndPush(i);
            cout << "PUBLISHER C++ : " << dec << i << " setValueAndPush()" << endl;
            this_thread::sleep_for(500ms);
        }
        this_thread::sleep_for(3s);
        publisher.notifyAll();
        cout << "PUBLISHER C++ : notifyAll()"<< endl;
        this_thread::sleep_for(3s);
        publisher.setValue(99);
        cout << "PUBLISHER C++ : 99 setValue()"<< endl;
        this_thread::sleep_for(4s);
        publisher.setValueAndNotifyOnChange(99);
        cout << "PUBLISHER C++ : 99 setValueAndNotifyOnChange()"<< endl;
        this_thread::sleep_for(1s);
        publisher.setValueAndNotifyOnChange(99);
        cout << "PUBLISHER C++ : 99 setValueAndNotifyOnChange()"<< endl;
        this_thread::sleep_for(1s);
        publisher.setValueAndNotifyOnChange(99);
        cout << "PUBLISHER C++ : 99 setValueAndNotifyOnChange()"<< endl;
        this_thread::sleep_for(1s);
        publisher.setValueAndNotifyOnChange(100);
        cout << "PUBLISHER C++ : 100 setValueAndNotifyOnChange()"<< endl;
        this_thread::sleep_for(5s);
    }
    return 0;
}