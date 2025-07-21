from SharedPubSub import *
import os
from time import sleep
## Watchout :
# If readWait() without timeout is in the main thread,
# Use ctrl+\ instead of ctrl+c to exit.

subscriber = Subscriber_int("Demo","DemoSubscriberPy",True)

while(True):
    os.system('clear')
    for i in range(0,8):
        value = subscriber.readWait()
        print("SUBSCRIBER PY:",(f"{value} readWait()" ) if value!=None else "readWait() No value in queue")
    sleep(3)
    for i in range(0,3):
        value = subscriber.readValue()
        print("SUBSCRIBER PY:",f"{value} readValue()" )
        sleep(1)
    
    print("SUBSCRIBER PY: waitForNotify()")
    subscriber.waitForNotify()
    
    value = subscriber.readValue()
    print("SUBSCRIBER PY:",f"{value} readValue()" )
    sleep(5)