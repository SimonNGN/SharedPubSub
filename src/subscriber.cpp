#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

class Test{
    public:
    int value1;
    float value2;
    atomic<int> value3;

    Test() : value1(0), value2(0.0f), value3(0) {}

    // Copy constructor
    Test(const Test& other)
        : value1(other.value1), value2(other.value2), value3(other.value3.load()) {}

    // Copy assignment
    Test& operator=(const Test& other) {
        if (this != &other) {
            value1 = other.value1;
            value2 = other.value2;
            value3.store(other.value3.load());
        }
        return *this;
    }

    ~Test() = default;
};

int main(){
    shps::Subscriber<Test> subscriber("TEST", "SUBSCRIBER1",true);

    while(1){
        Test t = subscriber.readWait().value();
        cout << dec << t.value1 << " " << t.value2 << " " << t.value3 <<endl;
        //this_thread::sleep_for(1s);
    }
    return 0;
}