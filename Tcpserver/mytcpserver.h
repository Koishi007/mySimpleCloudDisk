#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include "mytcpsocket.h"
#include <QMap>
#include <QThread>
#include <QString>
#include "recmsgworker.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();

    MyTcpServer* getTcpServerP();

    const QMap<QString, MyTcpSocket*>& getTcpSocketMap();

public slots:
    //void deletesocket(MyTcpSocket *mysocket);

    static MyTcpServer &getInstance();   //单例模式

    void incomingConnection(qintptr socketDescriptor);  //重写虚函数，有连接请求就会调用

    void addTcpSocket(QString userName, MyTcpSocket* mytcpsocket);

    void deleteTcpSocket(QString userName);

signals:


private:
    QMap<QString, MyTcpSocket*> m_tcpSocketMap;

};

#endif // MYTCPSERVER_H
