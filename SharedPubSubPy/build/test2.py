from SharedPubSubPy import *
import threading
import time

pub = PublisherFixedString("PYTHON")

i = 0

while 1:
    payload = FixedString()
    payload.set("Hello World!")
    pub.publish(payload)
    i+=1
    time.sleep(1)
    
    payload.set("Hello World! WITH MORE")
    pub.publish(payload)
    i+=1
    time.sleep(1)