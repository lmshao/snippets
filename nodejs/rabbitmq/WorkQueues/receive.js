var amqp = require('amqplib/callback_api');

amqp.connect('amqp://liming:12345678@localhost:5672', function (error, connection) {
    if (error) {
        throw error;
    }

    connection.createChannel(function (error, channel) {
        if (error) {
            throw error;
        }

        var queue = 'hello';

        channel.assertQueue(queue, {
            durable: true
        });

        console.log('[*] Waiting for message in %s. To exit press CTRL+C', queue);

        channel.consume(queue, function (msg) {
            console.log('[x] Received %s', msg.content.toString());
        }, {
            noAck: true
        });
    });

});