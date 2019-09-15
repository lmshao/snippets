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
        var msg = process.argv.slice(2).join(' ') || 'Hello World';

        channel.assertExchange(exchange, 'fanout', {
            durable: false
        });

        channel.publish(exchange, '', Buffer.from(msg));
        console.log("[x] Sent '%s'", msg);
    });

    setTimeout(function () {
        connection.close();
        process.exit(0);
    }, 500);
});