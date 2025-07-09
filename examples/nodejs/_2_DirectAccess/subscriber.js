const SharedPubSub = require('../build/Release/SharedPubSub');

const subscriber = new SharedPubSub.Subscriber_int("DirectAccess", "DirectAccessSubscriberJS", true);

(async () => {
    while (true) {
        /*--------- Wait for notification ---------*/
        let value = subscriber.readWait();
        console.log(`SUBSCRIBER : ${value}`);
        await new Promise(resolve => setTimeout(resolve, 1000));
    }
})();