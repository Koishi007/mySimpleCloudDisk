#include "recmsgworker.h"



recMsgWorker::recMsgWorker(qintptr socketDescriptor, QObject *parent) : QObject(parent)
{
    this->socketDescriptor = socketDescriptor;
}

recMsgWorker::~recMsgWorker()
{
    m_tcpSocket->deleteLater();
    qDebug()<<"Worker析构";
}

void recMsgWorker::doWork()
{
    m_tcpSocket = new MyTcpSocket;                                    //建立socket对象
    m_tcpSocket->setSocketDescriptor(socketDescriptor);              //把返回的socket描述符设置到创建的socket对象中，就可以通过这个socket来处理请求了
    connect(m_tcpSocket, &MyTcpSocket::disconnected, [=](){
        emit clientDisconnect();
    });
    emit newClinentConnect();
}


