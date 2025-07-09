const SharedPubSub = require('../build/Release/SharedPubSub');

const subscriber = new SharedPubSub.Subscriber_String("FixedString", "FixedStringSubscriberJS", true);
var value;

async function loop() {
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        value = subscriber.readWait();
        // Verify if the queue had a value on notification.
        if(value !== null && value !== undefined){
            console.log(`SUBSCRIBER : ${value}`);
        }
        else{
            console.log("SUBSCRIBER : No value in queue");
        }

        /*--------- Example 2 : Wait for notification with timeout ---------*/
        value = subscriber.readWaitMS(2000);
        // Verify if the queue had a value on notification.
        if(value !== null && value !== undefined){
            console.log(`SUBSCRIBER : ${value}`);
        }
        else{
            console.log("SUBSCRIBER : No value in queue");
        }
    }
}

loop().catch(console.error);