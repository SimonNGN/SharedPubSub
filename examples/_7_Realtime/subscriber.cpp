#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"
using namespace std;

/*
    NOTES
    -------
    To be as real time as possible,
    the real time process needs to :
    - Be in a real-time operating system
    - On a isolated core
    - Having only its process running on the core
    - At a fixed maximum CPU speed
    - Its priority to be the highest possible
    - No kernel interrupt on the same core.

    More options are sometimes needed like no_hz and rcu_nocbs.
    You also need to tune your kernel driver's interrupt priority.
    Research on the subject, on your needed platform.

*/

// Loop parameter. Slow timing for example.
const chrono::milliseconds loopTime = 250ms;
const chrono::microseconds wakeBefore = 500us;

int main(){

    shps::Subscriber<atomic<int>> subscriber("Realtime", "RealtimeSubscriber1");
    atomic<int>* value = subscriber.rawValue();
    auto nextLoop = chrono::steady_clock::now();

    while(1){
        cout << value->load() << endl; //cout is not real-time safe, but it's just to print an example
        nextLoop+=loopTime-wakeBefore;
        this_thread::sleep_until(nextLoop);
        while(chrono::steady_clock::now()<nextLoop){;}
    }
    return 0;
}