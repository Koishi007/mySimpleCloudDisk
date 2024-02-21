# mySimpleCloudDisk

一个简易的网盘系统用户可以上传文件至服务端，服务端可以保存文件至本地文件系统(利用QT的QFile)，客户端之间还可以进行聊天。以上功能的实现都基于QTcpServer和QTcpSocket。

客户端使用了qtmaterial的控件来美化了一下。

服务端简单使用了qt的多线程库，可以同时接受多个客户端的请求。

server.config与client.config分别是服务端和客户端的配置，只有ip和端口号，若需要本地运行记得修改为对应ip

客户端运行于Windows，服务端运行于Linux

代码中硬编码的路径部分，需要自行修改

目前可以在主机和虚拟机之间连接（虚拟机Linux）

以后有机会实现真正的主机间互相通信

代码水平有限，请见谅