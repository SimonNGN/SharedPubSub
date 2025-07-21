from SharedPubSub import *
from time import sleep

publisher = Publisher_FixedString2048("FixedString")
value = FixedString2048()

while(True):

    ### Example 1 : Publish ###
    value.set("EXAMPLE 1")
    publisher.publish(value)
    print("PUBLISHER PY :", value.get(), "Normal publish")
    sleep(1)

    ### Example 2 : Publish only on change ###

    for i in range(0,100):
        publisher.publishOnChange(value)

    value.set("EXAMPLE 2")
    publisher.publishOnChange(value)
    print("PUBLISHER PY :", value.get(), "Published on change")
    sleep(1)

    ### Example 3 : Push multiple in queue and notify after ###
    for i in range(0,10):
        value.set("EXAMPLE 3 "+str(i))
        publisher.setValueAndPush(value)
        print("PUBLISHER PY :", value.get())
    
    publisher.notifyAll()
    print("PUBLISHER PY :", value.get(),"Sent Multiple")
    sleep(1)