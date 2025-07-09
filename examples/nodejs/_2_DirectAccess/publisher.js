const SharedPubSub = require("SharedPubSub");
const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

const publisher = new SharedPubSub.Publisher_int("DirectAccess");
var value = 0;
async function loop(){
    while(1){
        // Modify the value directly
        value += 1;
        publisher.setValue(value);
        console.log(`PUBLISHER JS: ${value} setValue()`);
        await sleep(1000);
    }
}
loop();