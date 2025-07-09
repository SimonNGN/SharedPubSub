const SharedPubSub = require('../build/Release/SharedPubSub');

const publisher = new SharedPubSub.Publisher_String("FixedString");
let fixedString = "Initial Value";

async function loop() {
    while (true) {
        /*--------- Example 1 : Publish fixed string ---------*/
        publisher.publish(fixedString);
        console.log(`PUBLISHER : ${fixedString}`);
        fixedString = `Updated Value ${Math.floor(Math.random() * 100)}`; // Update the string value
        await new Promise(resolve => setTimeout(resolve, 1000)); // Wait for 1 second

        /*--------- Example 2 : Publish only on change ---------*/
        const newValue = `Updated Value ${Math.floor(Math.random() * 100)}`;
        if (newValue !== fixedString) {
            fixedString = newValue;
            publisher.publishOnChange(fixedString);
            console.log(`PUBLISHER : ${fixedString} (Published on change)`);
        }
        await new Promise(resolve => setTimeout(resolve, 1000)); // Wait for 1 second
    }
}

loop().catch(console.error);