const SharedPubSub = require('sharedpubsub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const subscriber = new SharedPubSub.Subscriber_atomic_int("DirectAccess", "DirectAccessSubscriberJS", true);
var value = 0;

async function loop() {
    while(1){
        /*--------- Example 1 : Read Value  ---------*/
        value = subscriber.readValue();
        console.log(`SUBSCRIBER JS : ${value} readValue()`);
        await sleep(1000);
    }
}

loop().catch(console.error);