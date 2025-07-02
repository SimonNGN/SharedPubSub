from SharedPubSubPy import *
import threading
import time

pub = PublisherFixedString("PYTHON")

i = 0

while 1:
    payload = FixedString()
    payload = "Hello World!"
    pub.publish(payload)
    i+=1
    time.sleep(1)
    
    payload = "Hello World!"("Hello World!")
    pub.publish(payload)
    i+=1
    time.sleep(1)