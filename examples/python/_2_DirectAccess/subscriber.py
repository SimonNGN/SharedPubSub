from SharedPubSub import *
## Watchout :
# If readWait() without timeout is in the main thread,
# Use ctrl+\ instead of ctrl+c to exit.

subscriber = Subscriber_int("PubSub","PubSubSubscriberPy",True)

while(True):
    ### Example 1 : Wait for notification ###
    value = subscriber.readWait()
    print("SUBSCRIBER :",value if value else "No value in queue")

    ### Example 2 : Wait for notification with a timeout ###
    value = subscriber.readWait(2000) # in milliseconds
    print("SUBSCRIBER :",value if value else "No value in queue")