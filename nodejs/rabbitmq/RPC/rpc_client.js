var amqp = require('amqplib/callback_api');

var args = process.argv.slice(2);

if (args.length == 0) {
    console.log("Usage: rpc_client.js num");
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
        channel.assertQueue('', {
            exclusive: true
        }, function (error2, q) {
            if (error2) {
                throw error2;
            }

            var correlationId = generateUuid();
            var num = parseInt(args[0]);
            console.log('[x] Requesting fib(%d)', num);

            channel.consume(q.queue, function (msg) {
                if (msg.properties.correlationId == correlationId) {
                    console.log('[.] Got %s', msg.content.toString());
                    setTimeout(function () {
                        connection.close();
                        process.exit(0)
                    }, 500);
                }
            }, {
                noAck: true
            });

            // 发送消息，同时把自己的Queue作为参数传递过去
            channel.sendToQueue('rpc_queue', Buffer.from(num.toString()), {
                correlationId: correlationId,
                replyTo: q.queue
            });
        });
    });
});

function generateUuid() {
    return Math.random().toString() + Math.random().toString() + Math.random().toString();
}