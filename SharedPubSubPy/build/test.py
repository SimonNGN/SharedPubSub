from SharedPubSubPy import *
import threading
import time

def test():

    sub = SubscriberString1024("PYTHON","PYTHONSUB",True)
    while 1:
        value = sub.readValue()
        print(value.getString())
        print(value.getSize())
        time.sleep(1)

i = 0
t = threading.Thread(target=test)
t.daemon = True
t.start()
while 1:
    print("TEST",i)
    i+=1
    time.sleep(1)