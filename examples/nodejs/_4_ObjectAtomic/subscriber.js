const SharedPubSub = require('../build/Release/SharedPubSub');

const subscriber = new SharedPubSub.Subscriber_Object("AtomicObject", "AtomicObjectSubscriberJS", true);
var atomicObject;

async function loop() {
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        atomicObject = subscriber.readWait();
        // Verify if the queue had a value on notification.
        if(atomicObject !== null && atomicObject !== undefined){
            console.log(`SUBSCRIBER : Value1: ${atomicObject.value1}, Value2: ${atomicObject.value2}, Value3: ${atomicObject.value3}`);
        }
        else{
            console.log("SUBSCRIBER : No value in queue");
        }

        /*--------- Example 2 : Wait for notification with timeout ---------*/
        atomicObject = subscriber.readWaitMS(2000);
        // Verify if the queue had a value on notification.
        if(atomicObject !== null && atomicObject !== undefined){
            console.log(`SUBSCRIBER : Value1: ${atomicObject.value1}, Value2: ${atomicObject.value2}, Value3: ${atomicObject.value3}`);
        }
        else{
            console.log("SUBSCRIBER : No value in queue");
        }
    }
}

loop().catch(console.error);