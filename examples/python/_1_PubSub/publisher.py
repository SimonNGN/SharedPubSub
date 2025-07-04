from SharedPubSub import *
from time import sleep

publisher = Publisher_int("PubSub")
value = 0

while(True):

    ### Example 1 : Publish ###
    value+=1
    publisher.publish(value)
    print("PUBLISHER PY :", value, "Normal publish")
    sleep(1)

    ### Example 2 : Publish only on change ###

    for i in range(0,100):
        publisher.publishOnChange(value)

    value+=1
    publisher.publishOnChange(value)
    print("PUBLISHER PY :", value, "Published on change")
    sleep(1)

    ### Example 3 : Push multiple in queue and notify after ###
    for i in range(0,10):
        value+=1
        publisher.setValueAndPush(value)
        print("PUBLISHER PY :", value)
    
    publisher.notifyAll()
    print("PUBLISHER PY :", value,"Sent Multiple")
    sleep(1)