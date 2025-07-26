const SharedPubSub = require('sharedpubsub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const subscriber = new SharedPubSub.Subscriber_FixedString2048("FixedString", "FixedStringSubscriberJS", true);

async function loop() {
    while(1){
        /*--------- Example 1 : Wait for notification ---------*/
        let value = await subscriber.readWaitAsync();
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

loop().catch(console.error);