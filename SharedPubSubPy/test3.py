from SharedPubSubPy import *
import threading
import time

pub = Publisher_atomic_int16("PYTHON")

i = 0

while 1:
    pub.publish(i)
    i+=1
    time.sleep(1)
    
    pub.publish(i)
    i+=1
    time.sleep(1)