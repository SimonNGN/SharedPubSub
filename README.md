# SharedPubSub
Provides Publisher and Subscriber classes for lock-free inter-process communication using POSIX shared memory with direct access, queues and notification.

## Main features
- Lock-free at runtime. Ideal for real-time operation.
- Event driven notification ; no need to poll for data.
- Can use atomic types for main data, will automatically use the non-atomic version for queues and readings.
- Templated, meaning you can share normal data, structs, objects, etc.
- Multiple subscribers to one publisher.
- Publisher can send data to subscriber's queue to read data in order.
- Publishers and Subscribers also have direct access to data for custom loop timing ; Subscriber can read the current value at any time.
- Publishers and Subscribers can exit and come back at any time because the data persists in shared memory.
## Main use cases
- Sharing data from a real-time loop to other threads/processes.
- Being able to read data at any time, as opposed to MQTT which is only event driven. Ideal for multiple process that don't need the data at the same time or their processing time are different.
- Receive data in order to make sure no data changes are missed.

## Requirements
- A POSIX environment (Most Linux distros)
- C++20
## How to import to a project
Add `SharedPubSub.hpp` header file to your project and include it.
## Functions
### Publisher :
|Function|Description|Usecase
|---|---|---|
|`publish`|Set current value.<br>Push value to subscribers' queue.<br>Notify subscribers.|Set and send value to subscribers|
|`publishOnChange`|Same as publish, but only if the new value is different from the previous value.|Set and send value to subscribers only on change|
|`readValue`|Returns a copy of the topic's value.|To read before modifying the value. Useful if the publisher quits and comes back.|
|`setValue`|Set the current topic's value.|If we don't need to notify the subscribers, like if they do direct access.| 
|`setValueAndNotifyOnChange`|Set the current topic's value and notify the subscribers.|If subscribers do direct access but still wants to get notified on change.|
|`setValueAndPush`|Set the current topic's value.<br>Push value to subcribers' queue.|To send multiple values into subscribers' queue to notify them later so they can consume all at once or let them consume at their own pace.|
|`notifyAll`|To notify all subscribers.|If we just simply want to notify.|
|`push`|Send a value to subscribers' queue.|If we want to send value without setting the topic's value.|
|`rawValue`|returns a raw pointer to the topic's value.|To have direct access to the value. If publisher and subscribers have direct access to an atomic<> type or struc/object, they can use the value safely.|

## Subscriber
|Function|Description|Usecase
|---|---|---|
|`subscribe`|Opens a queue in the topic.|Enables the subscriber to get notified and read values in a queue.|
|`readValue`|Returns a copy of the topic's value.|To read the current topic's value without the queue.|
|`readWait`|Pops a value in the queue.<br>If no value,waits indefinitely for notification.<br>Pops a value in the queue.|If we want to consume the queue or wait for a value in the queue without polling or a spinloop.|
|`readWait(timeout)`|Same as readWait, but with a timeout.|If we want to make sure the program doesn't get stuck waiting forever.|
|`waitForNotify`|Simply wait for notification.|If the subscriber uses direct access but still wants to get notified.|
|`waitForNotify(timeout)`|Same as waitForNotify, but with a timeout.|If we want to make sure the program doesn't get stuck waiting forever.|
|`rawValue`|returns a raw pointer to the topic's value.|To have direct access to the value. If publisher and subscribers have direct access to an atomic<> type or struc/object, they can use the value safely.|

## How to build examples
In the main folder :
- `mkdir build && cd build`
- `cmake .. && make`

Examples will be in their respective folders.

## Pub/Sub Example
Note : Pub/Sub is not the only mechanism, please look at the `examples` folder.
## Wish-list
- Give cross-compatible example with Python
- Give cross-compatible example with Javascript
- Make it compatible with Windows and Mac