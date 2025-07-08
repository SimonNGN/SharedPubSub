const SharedPubSub = require('../build/Release/SharedPubSub');

// Create a publisher for integers
const publisher = new SharedPubSub.Publisher_int("PubSub");
var value = 0;

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function loop() {
    while (1) {
        /*--------- Example 1 : Publish ---------*/
        publisher.publish(++value);
        console.log(`PUBLISHER NODEJS : ${value} Normal publish`);
        await sleep(1000);
        
        /*--------- Example 2 : Publish only on change ---------*/
        for(let i=0;i<100;++i){
            publisher.publishOnChange(value);
        }
        publisher.publishOnChange(++value);
        console.log(`PUBLISHER NODEJS : ${value} Published on change`);
        await sleep(1000);

        /*--------- Example 3 : Push multiple in queue and notify after ---------*/
        for(let i=0;i<10;++i){
            publisher.setValueAndPush(++value);
            console.log(`PUBLISHER NODEJS : ${value}`);
        }
        publisher.notifyAll();

        console.log(`PUBLISHER NODEJS : ${value} Sent multiple`);
        await sleep(1000);
        
    }
}

loop();