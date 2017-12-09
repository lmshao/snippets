>My private code base.   
>Linux C study notes.

# 1 UDP
## 1.1 UdpServer
UDP服务器：可以同时接收多个客户端的数据。
## 1.2 UdpClient
UDP客户端：以1s间隔持续发送一个字符串。
## 1.3 UdpClient_InputSend
UDP客户端：手动输入需要发送的字符串。

# 2 TCP
## 2.1 TcpServer
TCP服务器：同时只能接受一个客户端的请求。
## 2.2 TcpClient
TCP客户端：以1s间隔持续发送一个字符串。
## 2.3 TcpClient_InputSend
TCP客户端：手动输入需要发送的字符串。
## 2.4 TcpServer_InputSendRecv
TCP服务器：接收数据的同时也可以发送数据。
## 2.5 TcpClient_InputSendRecv
TCP客户端：发送数据的同时也可以接收数据。

# 3 Thread Synchronization
## 3.1 ThreadSynMutex
多线程同步：互斥锁
## 3.2 ThreadSynCondition
多线程同步：条件变量
## 3.3 ThreadSynSemaphore
多线程同步：信号量

# 4 I/O multiplexing
## 4.1 Select_TcpServer
此程序通过select函数实现可以同时接收多个客户端连接的TCP服务器。   
select函数的timeout位有三种设置：1.阻塞型设为NULL；2.设置非零时间，等待一段时间；3.轮询不等待，时间设为0。
## 4.2 Poll_TcpServer
此程序通过poll函数实现可以同时接收多个客户端连接的TCP服务器。   
select函数的timeout位有三种设置：1.阻塞型设为-1；2.设置非零时间，等待一段时间ms；3.轮询不等待，时间设为0。

# 5 杂项
## 5.1 GetDateTime
获取并处理日期和时间的函数。
