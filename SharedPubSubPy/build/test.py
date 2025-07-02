from SharedPubSubPy import *
import threading
import time

def test():

    sub = SubscriberFixedString("PYTHON","PYTHONSUB",True)
    while 1:
        value = sub.readValue()
        print(value.get())
        print(value.size())
        time.sleep(1)

i = 0
t = threading.Thread(target=test)
t.daemon = True
t.start()
while 1:
    print("TEST",i)
    i+=1
    time.sleep(1)