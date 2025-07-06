from SharedPubSub import *
from time import sleep
# Note : 
#   We will not do a realtime loop in this example.
#   We will simply subscribe to the realtime publisher and
#   Print on notification     
subscriber = Subscriber_atomic_int("Realtime","RealtimeSubscriberPy",True)

while(True):
    ### Example 1 : Read the value directly ###
    value = subscriber.readValue() # in milliseconds
    print("SUBSCRIBER :",value)
    sleep(0.25)