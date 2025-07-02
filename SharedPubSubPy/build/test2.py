from SharedPubSubPy import *
import threading
import time

pub = PublisherString1024("PYTHON")

i = 0

while 1:
    payload = SharedString1024()
    payload.setString("Hello World!")
    pub.publish(payload)
    i+=1
    time.sleep(1)
    payload.setString("Hello World!")
    pub.publish(payload)
    i+=1
    time.sleep(1)