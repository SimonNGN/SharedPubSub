const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const subscriber = new SharedPubSub.Subscriber_int("PubSub", "PubSubSubscriberJS", true);
var value;

async function loop() {
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        value = subscriber.readWait();
        // Verify if the queue had a value on notification.
        if(value !== null && value !== undefined){
            console.log(`SUBSCRIBER JS : ${value}`);
        }
        else{
            console.log("SUBSCRIBER JS : No value in queue");
        }

        /*--------- Example 2 : Wait for notification with timeout ---------*/
        value = subscriber.readWaitMS(2000);
        // Verify if the queue had a value on notification.
        if(value !== null && value !== undefined){
            console.log(`SUBSCRIBER JS : ${value}`);
        }
        else{
            console.log("SUBSCRIBER JS : No value in queue");
        }
    }
}

loop().catch(console.error);