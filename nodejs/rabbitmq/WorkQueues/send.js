var amqp = require('amqplib/callback_api');

amqp.connect('amqp://liming:12345678@localhost:5672', function (error, connection) {
    if (error) {
        throw error;
    }

    connection.createChannel(function (error, channel) {
        if (error) {
            throw error;
        }

        let queue = 'task_queue';
        let msg =  process.argv.slice(2).join(' ') || 'Hello World';

        channel.assertQueue(queue, {
            durable: true
        });

        channel.sendToQueue(queue, Buffer.from(msg), {
            persistent: true
        });
        console.log("[x] Sent '%s'", msg);
    });

    setTimeout(function () {
        connection.close();
        process.exit(0);
    }, 500);

});