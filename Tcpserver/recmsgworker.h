#ifndef RECMSGWORKER_H
#define RECMSGWORKER_H

#include <QObject>
#include "mytcpsocket.h"

class recMsgWorker : public QObject
{
    Q_OBJECT
public:
    explicit recMsgWorker(qintptr socketDescriptor, QObject *parent = nullptr);
    ~recMsgWorker();

public slots:
    void doWork();
signals:
    void clientDisconnect();
    void newClinentConnect();
private:
    qintptr socketDescriptor;
    MyTcpSocket *m_tcpSocket;
};

#endif // RECMSGWORKER_H
