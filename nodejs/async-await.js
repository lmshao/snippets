//
// Created by Liming SHAO on 2019/10/28.
//

'use strict';
let f1 = function(){
    return new Promise(((resolve, reject) => {
        setTimeout(function () {
            if (Math.random() < 0.9) {
                console.log('I am f1');
                resolve(new Date());
            } else {
                reject(new Error('> 0.9'));
            }
        }, 2000);
    }));
};

let f2 =  async function () {
    try {
        let ret2 = await f1();
        let ret3 = await f1();
        console.log('ret2 ' + ret2);
        console.log('ret3 ' + ret3);
    } catch (e) {
        console.log('catch: ' + e.toString());
    }
};

f2();

// await 必须在 async 函数里面，
// await 后面接的函数必须为Promise，
// await 外面必须有 try catch 捕捉 Promise.reject