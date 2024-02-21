#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include <QBitArray>
#include <QDebug>
#include <QStringList>
#include <QHostAddress>
#include <QMessageBox>
#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();

    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);      //启动tcp服务器，并监听
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");       //冒号开头的路径代表资源文件的路径 ./开头是当前文件路径
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();

        file.close();

        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");   //分割字符串
        this->m_strIP = strList[0];
        this->m_usPort = strList[1].toUShort();
        qDebug()<<m_strIP<<m_usPort;
    }
    else{
        qDebug()<<"file open error";
    }
}

