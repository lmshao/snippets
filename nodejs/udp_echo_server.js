import dgram from 'dgram';

const udp_server = dgram.createSocket('udp4');
udp_server.bind(6666);

udp_server.on('listening', function () {
    console.log('UDP server listening 6666.');
})

udp_server.on('message', function (msg, rinfo) {
    let strmsg = msg.toString();
    console.log(`From ${rinfo.address}:${rinfo.port} : ${strmsg}`)
    udp_server.send(strmsg, 0, strmsg.length, rinfo.port, rinfo.address);
    console.log('echo ok');
})

udp_server.on('error', function (err) {
    console.log('some error on udp server.')
    udp_server.close();
})