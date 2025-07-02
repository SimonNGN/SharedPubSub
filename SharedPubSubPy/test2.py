from SharedPubSubPy import *
import threading
import time

sub = Subscriber_FixedString2048("PYTHON","PYTHONSUB",True)

while 1:
    time.sleep(1)
    value = sub.readWait()
    if value:
        print(value.get())
    else:    
        print(sub.readWait())