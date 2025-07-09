const SharedPubSub = require('../build/Release/SharedPubSub');

const publisher = new SharedPubSub.Publisher_int("Realtime");
let value = 0;

async function loop() {
    while (true) {
        // Publish the current value
        publisher.publish(value);
        console.log(`PUBLISHER : ${value} Normal publish`);
        value += 1; // Increment the value for the next publish
        await new Promise(resolve => setTimeout(resolve, 1000)); // Wait for 1 second

        // Example of publishing only on change
        if (value % 5 === 0) { // Publish only if the value is a multiple of 5
            publisher.publishOnChange(value);
            console.log(`PUBLISHER : ${value} Published on change`);
        }

        // Example of pushing multiple values in queue and notifying after
        for (let i = 0; i < 3; i++) {
            value += 1;
            publisher.setValueAndPush(value);
            console.log(`PUBLISHER : ${value}`);
        }
        publisher.notifyAll();
        console.log(`PUBLISHER : ${value} Sent Multiple`);
        await new Promise(resolve => setTimeout(resolve, 1000)); // Wait for 1 second
    }
}

loop().catch(console.error);