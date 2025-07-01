from SharedPubSubPy import *
import threading
import time

pub = PublisherString1024("PYTHON")

i = 0

while 1:
    payload = SharedString1024()
    payload.value = "String Value " + str(i)
    pub.publish(payload)
    i+=1
    time.sleep(1)