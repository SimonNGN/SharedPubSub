const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const subscriber = new SharedPubSub.Subscriber_int("Demo", "DemoSubscriberJS", true);
var value;

async function loop() {
    while(1){
        console.clear();

        for(let i=0;i<8;i++){
            value = await subscriber.readWaitAsync();
            if(value !== null && value !== undefined){
                console.log(`SUBSCRIBER JS : ${value} readWaitAsync()`);
            }
            else{
                console.log("SUBSCRIBER JS : No value in queue");
            }
        }
        await sleep(3000);
        for(let i=0;i<3;i++){
            value = subscriber.readValue();
            console.log(`SUBSCRIBER JS : ${value} readValue()`);
            await sleep(1000);
        }

        console.log(`SUBSCRIBER JS : waitForNotifyAsync()`);
        await subscriber.waitForNotifyAsync()
        value = subscriber.readValue();
        console.log(`SUBSCRIBER JS : ${value} readValue()`);
        await sleep(5000);
    }
}

Promise.all([
    loop().catch(console.error),
])