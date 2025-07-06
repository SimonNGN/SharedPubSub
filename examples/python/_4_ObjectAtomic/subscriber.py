from SharedPubSub import *
from time import sleep

subscriber = Subscriber_ExampleClass("ObjectAtomic","ObjectAtomicSubscriberPy",True)

while(True):
    ### Example 1 : Wait for notification ###
    exampleObject = subscriber.readWait()
    if exampleObject:
        exampleObject.printValues()
    else:
        ### Example 2 : Nothing was in queue, it was a normal notify, read the current value ###
        # Thread safe because all members are atomic
        # With raw value, we can also modify the values safely if we want
        exampleObject = subscriber.rawValue()
        exampleObject.printValues()
