#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QTcpSocket>
#include "protocol.h"
#include <QHash>
#include <QString>
#include "connectDB.h"

#include <QFile>
#include <QTimer>

#define LOCAL_PATH "/home/koishi/extraDisk/userData/"       //服务器存储用户数据路径
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();

    ~MyTcpSocket();

    qint64 calTotalDirSize(const QString &path, const QString &t_strFileName);

signals:
    void offLine(QString userName);

    void onLine(QString userName, MyTcpSocket *mysocket);

    void sendPrivateMsg(QString senderName, QString msg);

public slots:
    void rec_Msg();

    void clientOffline();

    void sendFileToClient();

    void recPrivateMsg(QString senderName, QString msg);

private:
    QString m_UserName;
    QFile m_file;
    QTimer *m_timer;
    qint64 m_fileTotalSize;         //接收文件的总大小
    qint64 m_fileRecvedSize;           //已经接收的大小
    bool m_isUpload;


};

#endif // MYTCPSOCKET_H
