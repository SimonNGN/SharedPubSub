#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

struct CustomStruct{
    int value1 = 0;
    float value2 = 0.0;
    long int value3 = 42;
};

int main(){
    
    shps::Publisher<CustomStruct> publisher("Struct");
    CustomStruct customStruct;
    while(1){
        /*--------- Example 1 : Publish  ---------*/
        customStruct = publisher.readValue();
        customStruct.value1+=1;
        customStruct.value2+=0.1;
        customStruct.value3+=10;
        publisher.publish(customStruct);
        cout << "PUBLISHER : " << dec << 
        customStruct.value1 << ", " <<
        customStruct.value2 << ", " <<
        customStruct.value3 << "" <<
        endl;
        this_thread::sleep_for(1s);

        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        CustomStruct* rawCustomStruct = publisher.rawValue();
        rawCustomStruct->value1+=1;
        rawCustomStruct->value2+=0.1;
        rawCustomStruct->value3+=10;
        publisher.notifyAll();

        cout << "PUBLISHER : " << dec << 
        rawCustomStruct->value1 << ", " <<
        rawCustomStruct->value2 << ", " <<
        rawCustomStruct->value3 << " RAW VALUE" <<
        endl;
        this_thread::sleep_for(1s);
    }
    return 0;
}