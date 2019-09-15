var amqp = require('amqplib/callback_api');

var args = process.argv.slice(2);

if (args.length == 0) {
    console.log("Usage: receive_logs_topic.js [info] [warning] [error]");
    process.exit(1);
}

amqp.connect('amqp://liming:12345678@localhost:5672', function (error, connection) {
    if (error) {
        throw error;
    }

    connection.createChannel(function (error1, channel) {
        if (error1) {
            throw error1;
        }
        var exchange = 'direct_logs';
        channel.assertExchange(exchange, 'direct', {
            durable: false
        });

        channel.assertQueue('', {
            exclusive: true
        }, function (error2, q) {
            if (error2) {
                throw error2;
            }

            console.log('[*] Waiting for logs. To exit press Ctrl+C');
            args.forEach(function (severity) {
                channel.bindQueue(q.queue, exchange, severity);
            });

            channel.consume(q.queue, function (msg) {
                console.log("[x] %s: '%s'", msg.fields.routingKey, msg.content.toString());
            }, {
                noAck: true
            });
        });
    });
});