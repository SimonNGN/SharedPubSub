from SharedPubSub import *
from time import sleep

subscriber = Subscriber_atomic_int("DirectAccess","DirectAccessSubscriberPy",False)

while(True):
    ### Example 1 : Wait for notification ###
    value = subscriber.readValue()
    print("SUBSCRIBER PY:",value)
    sleep(1)