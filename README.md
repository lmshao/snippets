>My private code base.   
>Linux C study notes.

#1 UDP
##1.1 UdpServer
UDP服务器：可以同时接收多个客户端的数据。
##1.2 UdpClient
UDP客户端：以1s间隔持续发送一个字符串。
##1.3 UdpClient_InputSend
UDP客户端：手动输入需要发送的字符串。

#2 TCP
##2.1 TcpServer
TCP服务器：同时只能接受一个客户端的请求。
##2.2 TcpClient
TCP客户端：以1s间隔持续发送一个字符串。
##2.3 TcpClient_InputSend
TCP客户端：手动输入需要发送的字符串。

#3 Thread Synchronization
##3.1 ThreadSynMutex
多线程同步：互斥锁
##3.2 ThreadSynCondition
多线程同步：条件变量
##3.3 ThreadSynSemaphore
多线程同步：信号量