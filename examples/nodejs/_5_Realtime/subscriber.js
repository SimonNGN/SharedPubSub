const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

// Note : 
//   We will not do a realtime loop in this example.
//   We will simply subscribe to the realtime publisher and
//   Print on notification    

const subscriber = new SharedPubSub.Subscriber_atomic_int("Realtime", "RealtimeSubscriberJS", true);

async function loop() {
    while(1){
        let value = subscriber.readValue()
        console.log(`SUBSCRIBER JS : ${value}`);
        await sleep(250);
    }
}

loop().catch(console.error);