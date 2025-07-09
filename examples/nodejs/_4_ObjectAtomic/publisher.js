const SharedPubSub = require('../build/Release/SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const publisher = new SharedPubSub.Publisher_int("");

async function loop() {
    while (true) {
    }
}

loop().catch(console.error);