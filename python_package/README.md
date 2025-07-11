# SharedPubSub
Provides Publisher and Subscriber classes for lock-free inter-process communication using POSIX shared memory with direct access, queues and notification.

## Main features
- Lock-free at runtime. Ideal for real-time operation.
- Event driven notification ; no need to poll for data.
- Can use atomic types for main data, will automatically use the non-atomic version for queues and readings.
- Templated, meaning you can share normal data, structs, objects, etc.
- Cross-language compatible (C++,Python,Javascript(NodeJS) )
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
### Python
install the library `pip install SharedPubSub` and import it into your project.

## Functions (Python)
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
|`readWaitMS(timeout)`|Same as readWait, but with a timeout.|If we want to make sure the program doesn't get stuck waiting forever.|
|`waitForNotify`|Simply wait for notification.|If the subscriber uses direct access but still wants to get notified.|
|`waitForNotifyMS(timeout)`|Same as waitForNotify, but with a timeout.|If we want to make sure the program doesn't get stuck waiting forever.|
|`rawValue`|returns a raw pointer to the topic's value.|To have direct access to the value. If publisher and subscribers have direct access to an atomic<> type or struc/object, they can use the value safely.|

## How to build and run examples
Examples are compatible between languages
### Python
In the `python_package` folder :
- `pip install .` (You might want to do this in a venv)
- Examples are in the `examples/python` folder

## Pub/Sub Example
Note : This example is only one of many mechanism. Please look at the `examples` folder.

### Python
#### Publisher
```python
from SharedPubSub import *
from time import sleep

publisher = Publisher_int("PubSub")
value = 0

while(True):
    value+=1
    publisher.publish(value)
    print("PUBLISHER PY :", value, "Normal publish")
    sleep(1)
```
#### Subscriber
```python
from SharedPubSub import *

subscriber = Subscriber_int("PubSub","PubSubSubscriberPy",True)

while(True):
    value = subscriber.readWait()
    print("SUBSCRIBER :",value if value else "No value in queue")
```

## Classes
This library as some base classes and a custom string classes.<br>
YOU CAN IMPLEMENT YOUR OWN by changing the source code.

### Custom Classes
- `FixedString2048`
- `ExampleClass`
- `ExampleClassAtomic`

### Publisher Classes
- `Publisher_bool`
- `Publisher_int`
- `Publisher_uint`
- `Publisher_int8`
- `Publisher_uint8`
- `Publisher_int16`
- `Publisher_uint16`
- `Publisher_int64`
- `Publisher_uint64`
- `Publisher_float`
- `Publisher_double`
- `Publisher_FixedString2048`
- `Publisher_ExampleClass`

### Atomic Publisher Classes
- `Publisher_atomic_bool`
- `Publisher_atomic_int`
- `Publisher_atomic_uint`
- `Publisher_atomic_int8`
- `Publisher_atomic_uint8`
- `Publisher_atomic_int16`
- `Publisher_atomic_uint16`
- `Publisher_atomic_int64`
- `Publisher_atomic_uint64`
- `Publisher_atomic_float`
- `Publisher_atomic_double`
- `Publisher_ExampleClassAtomic`

### Subscriber Classes
- `Subscriber_bool`
- `Subscriber_int`
- `Subscriber_uint`
- `Subscriber_int8`
- `Subscriber_uint8`
- `Subscriber_int16`
- `Subscriber_uint16`
- `Subscriber_int64`
- `Subscriber_uint64`
- `Subscriber_float`
- `Subscriber_double`
- `Subscriber_FixedString2048`
- `Subscriber_ExampleClass`

### Atomic Subscriber Classes
- `Subscriber_atomic_bool`
- `Subscriber_atomic_int`
- `Subscriber_atomic_uint`
- `Subscriber_atomic_int8`
- `Subscriber_atomic_uint8`
- `Subscriber_atomic_int16`
- `Subscriber_atomic_uint16`
- `Subscriber_atomic_int64`
- `Subscriber_atomic_uint64`
- `Subscriber_atomic_float`
- `Subscriber_atomic_double`
- `Subscriber_ExampleClassAtomic`

## Things to watch out for
- The order in which the publisher and subscriber are created is not important, but if it is the FIRST time the shared memory is created, they cannot be created at the same time. Otherwise, there might be a race condition on the shared memory and the Topic object could be created twice, possibly causing the subscriberIndex to be 0 even though there is 1 subscriber. The recommended approach is to start one process first and make it a dependency for the other.
- There is a maximum number of values in a queue (which you can change). When the queue is full, the publisher will not push to it anymore. The subscriber needs to be able to consume the values faster than they are being published.
- All the data created in shared memory (/dev/shm) WILL PERSIST. The library does not destroy or clean the data, on purpose. That way, the publisher and subscriber can exit and come back at will and the data will still be valid. You have to manually handle cleaning if you want to.
## Wish list
- Give cross-compatible example with Javascript
- Make it compatible with Windows and Mac