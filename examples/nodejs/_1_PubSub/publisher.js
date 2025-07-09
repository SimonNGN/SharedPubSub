const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const publisher = new SharedPubSub.Publisher_int("PubSub");
let value = 0;

async function loop() {
    while (true) {
        /*--------- Example 1 : Publish ---------*/
        value += 1;
        publisher.publish(value);
        console.log(`PUBLISHER JS : ${value} Normal publish`);
        await new Promise(resolve => setTimeout(resolve, 1000));

        /*--------- Example 2 : Publish only on change ---------*/
        for (let i = 0; i < 100; i++) {
            publisher.publishOnChange(value);
        }

        value += 1;
        publisher.publishOnChange(value);
        console.log(`PUBLISHER JS : ${value} Published on change`);
        await sleep(1000);

        /*--------- Example 3 : Push multiple in queue and notify after ---------*/
        for (let i = 0; i < 10; i++) {
            value += 1;
            publisher.setValueAndPush(value);
            console.log(`PUBLISHER JS : ${value}`);
        }

        publisher.notifyAll();
        console.log(`PUBLISHER JS : ${value} Sent Multiple`);
        await sleep(1000);
    }
}

loop().catch(console.error);