from SharedPubSub import *
from time import sleep

publisher = Publisher_ExampleClass("Object")

while(True):

    ### Example 1 : Publish ###
    exampleObject = publisher.readValue()
    exampleObject.value1+=1
    exampleObject.value2+=0.1
    exampleObject.value3+=10
    publisher.publish(exampleObject)
    exampleObject.printValues()
    sleep(1)

    ### Example 2 : Get raw pointer, change value and notify ###
    rawExampleObject = publisher.rawValue()
    rawExampleObject.value1+=1
    rawExampleObject.value2+=0.1
    rawExampleObject.value3+=10
    publisher.notifyAll()
    rawExampleObject.printValues()
    sleep(1)
