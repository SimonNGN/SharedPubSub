const SharedPubSub = require('sharedpubsub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const publisher = new SharedPubSub.Publisher_atomic_int("DirectAccess");
var value=0;

async function loop() {
    while (true) {
        /*--------- Example 1 : Set Value  ---------*/
        publisher.setValue(++value);
        console.log(`PUBLISHER JS: ${value} setValue()`);
        await sleep(1000);
    }
}

loop().catch(console.error);