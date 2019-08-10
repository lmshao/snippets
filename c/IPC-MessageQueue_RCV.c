/*
 * System V IPC - MessageQueue send  
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
}MsgBuf;

int main() {

    int msgId = msgget((key_t)4444, 0666|IPC_CREAT);
    if (msgId == -1){
        printf("msgget err.\n");
        return 1;
    }

    for (;;) {
        if (msgrcv(msgId, (void *)&MsgBuf, BUFSIZ, 0, MSG_NOERROR) == -1) {
            printf("msgrcv err.\n");
            return 1;
        }

        printf("Recv: Type:%ld Msg:%s", MsgBuf.msgType, MsgBuf.text);

        if (strncmp(MsgBuf.text, "end", 3) == 0)
            break;
    }

    if(msgctl(msgId, IPC_RMID, 0) == -1){
        printf("msgctl err.\n");
        return 1;
    }

    printf("Over!\n");
    return 0;
}