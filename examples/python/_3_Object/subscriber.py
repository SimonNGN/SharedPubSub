from SharedPubSub import *
from time import sleep

subscriber = Subscriber_ExampleClass("Object","ObjectSubscriberPy",True)

while(True):
    ### Example 1 : Wait for notification ###
    exampleObject = subscriber.readWait()
    if exampleObject:
        exampleObject.printValues()
    else:
        ### Example 2 : Nothing was in queue, it was a normal notify, read the current value (not thread safe if not atomic) ###
        exampleObject = subscriber.readValue()
        exampleObject.printValues()
