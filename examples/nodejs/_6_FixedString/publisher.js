const SharedPubSub = require('SharedPubSub');
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const publisher = new SharedPubSub.Publisher_FixedString2048("FixedString");

async function loop() {
    while (true) {
        /*--------- Example 1 : Publish ---------*/
        let value = "EXAMPLE 1";
        publisher.publish(value);
        console.log(`PUBLISHER JS : ${publisher.readValue()} Normal publish`)
        await sleep(1000);

        /*--------- Example 2 : Publish only on change ---------*/
        for(let i=0;i<100;++i){
            publisher.publishOnChange("EXAMPLE 1");
        }
        publisher.publishOnChange("EXAMPLE 2");
        console.log(`PUBLISHER JS : ${publisher.readValue()} Published on change`)
        await sleep(1000);

        /*--------- Example 3 : Push multiple in queue and notify after ---------*/
        for (let i = 0; i < 10; i++) {
            value += 1;
            publisher.setValueAndPush(`EXAMPLE ${i}`);
            console.log(`PUBLISHER JS : ${publisher.readValue()}`);
        }

        publisher.notifyAll();
        console.log(`PUBLISHER JS : ${publisher.readValue()} Sent Multiple`);
        await sleep(1000);
    }
}

loop().catch(console.error);