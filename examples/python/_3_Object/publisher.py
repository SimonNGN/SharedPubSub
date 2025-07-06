from SharedPubSub import *
from time import sleep

publisher = Publisher_atomic_int("DirectAccess")
value = 0

while(True):

    ### Example 1 : Set Value ###
    value+=1
    publisher.setValue(value)
    print("PUBLISHER PY :", value, "setValue()")
    sleep(1)
