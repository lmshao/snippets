var amqp = require('amqplib/callback_api');

amqp.connect('amqp://liming:12345678@localhost:5672', function (error, connection) {
    if (error) {
        throw error;
    }

    connection.createChannel(function (error1, channel) {
        if (error1) {
            throw error1;
        }

        const exchange = 'logs';

        channel.assertExchange(exchange, 'fanout', {
            durable: false
        });

        channel.assertQueue('', {
            exclusive: true
        }, function (error2, q) {
            if (error2) {
                throw error2;
            }
            console.log("[*] Waiting for messages in '%s', To exit press Ctrl+C", q.queue);
            channel.bindQueue(q.queue, exchange, '');

            channel.consume(q.queue, function (msg) {
                if (msg.content) {
                    console.log("[x] '%s'", msg.content.toString());
                }
            }, {
                noAck: true
            });
        });
    });
});
