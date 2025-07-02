from SharedPubSubPy import *
import threading
import time

sub = Subscriber_atomic_int16("PYTHON","PYTHONSUB",True)

while 1:
    time.sleep(1)
    value = sub.readWait()
    if value:
        print(value)
    else:    
        print("NONE")