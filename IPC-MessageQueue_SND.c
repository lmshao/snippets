/*
 * System V IPC - MessageQueue receive  
 * int msgget(key_t key, int msgflg);
 * int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
 * ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
 * int msgctl(int msqid,int cmd,struct msqid_ds *buf);
 * Copyright (c) 2017 Liming Shao <lmshao@163.com>
 */

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct MyMsg{
    long int msgType;
    char text[BUFSIZ];
};

int main() {

    struct MyMsg MsgBuf;
    MsgBuf.msgType = 200; // note: must be defined
    int msgId = msgget((key_t)4444, 0666|IPC_CREAT);
    if (msgId == -1){
        printf("msgget err.\n");
        return 1;
    }

    for (;;) {
        printf("Input text:\n");
        fgets(MsgBuf.text, 512, stdin);
        if(msgsnd(msgId, (void *)&MsgBuf, BUFSIZ, IPC_NOWAIT) == -1){
            printf("msgsnd err.\n");
            break;
        }

        if (strncmp(MsgBuf.text, "end", 3) == 0)
            break;
    }

    return 0;
}