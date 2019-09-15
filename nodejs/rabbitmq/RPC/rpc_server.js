// 以普通生产者消费者模式传递消息，发送方发送消息同时把自己的Queue作为参数传递给消费者，消费者收到消息处理后再把结果发送到参数指定的发送者的Queue中

var amqp = require('amqplib/callback_api');

amqp.connect('amqp://liming:12345678@localhost:5672', function (error, connection) {
    if (error) {
        throw error;
    }

    connection.createChannel(function (error1, channel) {
        if (error1) {
            throw error1;
        }

        var queue = 'rpc_queue';
        channel.assertQueue(queue, {
            durable: false
        });

        channel.prefetch(1);
        console.log('[x] Awaiting RPC requests');

        channel.consume(queue, function (msg) {
            var n = parseInt(msg.content.toString());
            console.log("[.] fib(%d)", n);

            var r = (n >= 40) ? -1 : fibonacci(n);

            // 接收一个消息的时候再发送一个消息，发送到的消息队列是消息参数properties中指定的replyTo
            channel.sendToQueue(msg.properties.replyTo, Buffer.from(r.toString()), {
                correlationId: msg.properties.correlationId
            });

            channel.ack(msg);
        });

    });
});

function fibonacci(n) {
    if (n == 0 || n == 1)
        return n;
    else
        return fibonacci(n - 1) + fibonacci(n - 2);
}