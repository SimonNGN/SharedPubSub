const SharedPubSub = require('../build/Release/SharedPubSub');

const subscriber = new SharedPubSub.Subscriber_ExampleClass("Object", "ObjectSubscriberJS", true);

async function loop() {
    while (true) {
        /*--------- Example 1 : Wait for notification ---------*/
        const exampleObject = subscriber.readWait();
        if (exampleObject) {
            console.log(`SUBSCRIBER : Value1: ${exampleObject.value1}, Value2: ${exampleObject.value2}, Value3: ${exampleObject.value3}`);
        } else {
            console.log("SUBSCRIBER : No object in queue");
        }

        /*--------- Example 2 : Nothing was in queue, read the current value ---------*/
        const currentObject = subscriber.readValue();
        if (currentObject) {
            console.log(`SUBSCRIBER : Current Value1: ${currentObject.value1}, Value2: ${currentObject.value2}, Value3: ${currentObject.value3}`);
        } else {
            console.log("SUBSCRIBER : No current object available");
        }
    }
}

loop().catch(console.error);