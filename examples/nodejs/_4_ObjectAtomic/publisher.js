const SharedPubSub = require('../build/Release/SharedPubSub');
const { AtomicObject } = require('./atomicObject'); // Assuming you have an AtomicObject class defined for thread safety
const publisher = new SharedPubSub.Publisher_ObjectAtomic("AtomicObject");

const atomicObject = new AtomicObject();

setInterval(() => {
    // Modify the properties of the atomic object
    atomicObject.value1 += 1;
    atomicObject.value2 += 0.1;
    atomicObject.value3 += 10;

    // Publish the atomic object
    publisher.publish(atomicObject);
    atomicObject.printValues(); // Assuming printValues method exists to log the properties
}, 1000);