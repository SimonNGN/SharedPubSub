const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const publisher = new SharedPubSub.Publisher_ExampleClass("Object");

async function loop() {
    while (true) {
        /*--------- Example 1 : Publish  ---------*/
        let exampleObject = publisher.readValue();
        exampleObject.value1+=1;
        exampleObject.value2+=0.1;
        exampleObject.value3+=10;
        publisher.publish(exampleObject);
        exampleObject.printValues();
        await sleep(1000)
        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        // Although it's possible, we will not do raw pointer examples
    }
}

loop().catch(console.error);