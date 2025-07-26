const SharedPubSub = require('sharedpubsub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const subscriber = new SharedPubSub.Subscriber_ExampleClass("Object", "ObjectSubscriberJS", true);

async function loop() {
    while(true){
        /*--------- Example 1 : Wait for notification ---------*/
        let object1 = await subscriber.readWaitAsync();
        if(object1 !== null && object1 !== undefined){
            object1.printValues();
        }
        else{
            /*--------- Example 2 : Nothing was in queue, it was a normal notify, read the current value (not thread safe if not atomic) ---------*/
            let object2 = subscriber.readValue();
            object2.printValues();
        }
    }
}

loop().catch(console.error);