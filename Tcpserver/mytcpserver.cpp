#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer()
{

}

MyTcpServer *MyTcpServer::getTcpServerP()
{
    return this;
}

const QMap<QString, MyTcpSocket *> &MyTcpServer::getTcpSocketMap()
{
    return m_tcpSocketMap;
}


MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)      //服务器接收客户端请求时，调用该函数，生成一个socket,并返回描述符
{
      recMsgWorker* worker = new recMsgWorker(socketDescriptor);
      QThread* workerThread = new QThread;                          //为每一个连接的客户创建一个线程
      worker->moveToThread(workerThread);
      connect(workerThread, &QThread::started, worker, &recMsgWorker::doWork);
      connect(workerThread, &QThread::finished, worker, &recMsgWorker::deleteLater);
      connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
      connect(worker, &recMsgWorker::newClinentConnect, this, [=](){        //即使使用lambda也要加this，不然槽函数会在发出信号的那个线程直接执行
          qDebug()<<"新用户连接,尚未登陆";
      });
      connect(worker, &recMsgWorker::clientDisconnect, this, [=](){
          workerThread->quit();
          workerThread->wait();
          qDebug()<<"断开连接";
      });
      workerThread->start();
}

void MyTcpServer::addTcpSocket(QString userName, MyTcpSocket *mytcpsocket)
{
    m_tcpSocketMap.insert(userName, mytcpsocket);
    qDebug()<<"新用户连接,登陆,用户名："<<userName;
}

void MyTcpServer::deleteTcpSocket(QString userName)
{
    m_tcpSocketMap.remove(userName);
}
