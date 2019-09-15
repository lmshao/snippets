var amqp = require('amqplib/callback_api');

amqp.connect('amqp://liming:12345678@localhost:5672', function (error, connection) {
    if (error) {
        throw error;
    }

    connection.createChannel(function (error1, channel) {
        if (error1) {
            throw error1;
        }

        var queue = 'task_queue';

        channel.assertQueue(queue, {
            durable: true
        });

        channel.prefetch(1);
        console.log("[*] Waiting for message in %s. To exit press Ctrl+C", queue);

        channel.consume(queue, function (msg) {
            console.log("[x] Received '%s'", msg.content.toString());

            setTimeout(function () {
                console.log("[x] Done");
                channel.ack(msg);
            }, 1000);
        }, {
            noAck: false
        });
    });
});