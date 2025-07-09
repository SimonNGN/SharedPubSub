const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const subscriber = new SharedPubSub.Subscriber_int("", "SubscriberJS", true);

async function loop() {
    while(1){

    }
}

loop().catch(console.error);