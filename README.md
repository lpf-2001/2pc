# 2pc
分布式一致性协议 2pc 可能还有一些bug，写的很捞，没有加锁，后续可能会补上

## 如何运行
服务端 g++ main.cpp params.h -o server -lpthread -O0 加O0为了避免cpu乱序执行，也可以不加试试   然后./server 8008(后面是端口号，也可以换个端口，但是客户端代码的端口号也要改)
客户端 g++ client.cpp -o client -O0     再./client

服务器输入命令c便可开始一次投票，输入exit可退出服务器。

客户端输入exit模拟断开连接的情况。

服务端输出commit transaction 表示一次成功的提交
abort就失败