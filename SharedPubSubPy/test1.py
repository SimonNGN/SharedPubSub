from SharedPubSubPy import *
import threading
import time

pub = Publisher_FixedString2048("PYTHON")

i = 0

while 1:
    payload = FixedString2048()
    payload.set("HELLO WORLD " +str(i))
    pub.publish(payload)
    i+=1
    time.sleep(1)
    
    payload.set("Hello World!" +str(i) + "AGAIN")
    pub.publish(payload)
    i+=1
    time.sleep(1)