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
## Functions

### Publisher :
|Function|Description|Usecase
|---|---|---|
|`publish()`|Set current value<br>Push value to subscribers' queue<br>Notify subscribers|set and send values to subscribers| 

## How to build examples
In the main folder :
- `mkdir build && cd build`
- `cmake .. && make`

Examples will be in their respective folders.

## Wish-list
- Give cross-compatible example with Python
- Give cross-compatible example with Javascript
- Make it compatible with Windows and Mac