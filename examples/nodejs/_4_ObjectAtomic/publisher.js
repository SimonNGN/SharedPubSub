const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const publisher = new SharedPubSub.Publisher_ExampleClassAtomic("ObjectAtomic");

async function loop() {
    while (true) {
        /*--------- Example 1 : Publish  ---------*/
        let exampleObject = publisher.readValue();
        exampleObject.setValue1(exampleObject.getValue1()+1);
        exampleObject.setValue2(exampleObject.getValue2()+0.1);
        exampleObject.setValue3(exampleObject.getValue3()+10);
        publisher.publish(exampleObject);
        exampleObject.printValues();
        await sleep(1000)
        /*--------- Example 2 : Get raw pointer, change value and notify  ---------*/
        // Although it's possible, we will not do raw pointer examples
    }
}

loop().catch(console.error);