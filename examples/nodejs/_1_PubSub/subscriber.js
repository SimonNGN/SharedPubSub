const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const subscriber = new SharedPubSub.Subscriber_int("PubSub", "PubSubSubscriberJS", true);
var value;

async function loop() {
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        value = await subscriber.readWaitAsync();
        // Verify if the queue had a value on notification.
        if(value !== null && value !== undefined){
            console.log(`SUBSCRIBER JS : ${value}`);
        }
        else{
            console.log("SUBSCRIBER JS : No value in queue");
        }

        /*--------- Example 2 : Wait for notification with timeout ---------*/
        value = await subscriber.readWaitMSAsync(2000);
        // Verify if the queue had a value on notification.
        if(value !== null && value !== undefined){
            console.log(`SUBSCRIBER JS : ${value}`);
        }
        else{
            console.log("SUBSCRIBER JS : No value in queue");
        }
    }
}

// We add a print loop to demonstrate that it is possible to do async calls in javascript
async function printLoop(){
    while(true){
        console.log("Async Print");
        await sleep(500);
    }
}

Promise.all([
    loop().catch(console.error),
    printLoop().catch(console.error)
])