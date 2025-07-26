from SharedPubSub import *
from time import sleep

publisher = Publisher_ExampleClassAtomic("ObjectAtomic")

while(True):

    ### Example 1 : Publish ###
    exampleObjectAtomic = publisher.readValue()
    exampleObjectAtomic.setValue1(exampleObjectAtomic.getValue1()+1)
    exampleObjectAtomic.setValue2(exampleObjectAtomic.getValue2()+0.1)
    exampleObjectAtomic.setValue3(exampleObjectAtomic.getValue3()+10)
    publisher.publish(exampleObjectAtomic)
    exampleObjectAtomic.printValues()
    sleep(1)

    ### Example 2 : Get raw pointer, change value and notify ###
    rawExampleObjectAtomic = publisher.rawValue()
    rawExampleObjectAtomic.setValue1(rawExampleObjectAtomic.getValue1()+1)
    rawExampleObjectAtomic.setValue2(rawExampleObjectAtomic.getValue2()+0.1)
    rawExampleObjectAtomic.setValue3(rawExampleObjectAtomic.getValue3()+10)
    publisher.notifyAll()
    rawExampleObjectAtomic.printValues()
    sleep(1)
