const SharedPubSub = require('../build/Release/SharedPubSub');
const sleep = require('util').promisify(setTimeout);

const publisher = new SharedPubSub.Publisher_Object("Object");

async function loop() {
    let exampleObject = publisher.readValue();

    while (true) {
        // Modify the properties of the object
        exampleObject.value1 += 1;
        exampleObject.value2 += 0.1;
        exampleObject.value3 += 10;

        // Publish the modified object
        publisher.publish(exampleObject);
        exampleObject.printValues();

        await sleep(1000); // Sleep for 1 second

        // Get raw pointer, modify value and notify
        const rawExampleObject = publisher.rawValue();
        rawExampleObject.value1 += 1;
        rawExampleObject.value2 += 0.1;
        rawExampleObject.value3 += 10;

        publisher.notifyAll();
        rawExampleObject.printValues();

        await sleep(1000); // Sleep for 1 second
    }
}

loop().catch(console.error);