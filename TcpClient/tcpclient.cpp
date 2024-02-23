#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QBitArray>
#include <QDebug>
#include <QStringList>
#include <QHostAddress>
#include <QMessageBox>
#include <QHBoxLayout>
#include "protocol.h"
#include "operateWidget.h"


TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
    , m_isdownLoad(false)
{
    ui->setupUi(this);
    loadConfig();
    ui->contianer->setStyleSheet("QWidget { background: white; }");
    ui->name_line_edit->setLabel("用户名");
    ui->pwd_line_edit->setLabel("密码");

    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(rec_Msg()));

    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);        //连接服务器
    qDebug()<<m_strIP<<m_usPort;
}

TcpClient::~TcpClient()
{
    delete ui;
    qDebug()<<"Tcp析构";
}

void TcpClient:: loadConfig(){              //加载配置文件
    QFile file(":/config/client.config");       //冒号开头的路径代表资源文件的路径 ./开头是当前文件路径
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();

        file.close();

        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");   //分割字符串
        this->m_strIP = strList[0];
        this->m_usPort = strList[1].toUShort();
    }
    else{
        qDebug()<<"file open error";
    }
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

TcpClient *TcpClient::getTcpClientP()
{
    return this;
}

const QString &TcpClient::getUserName()
{
    return m_name;
}

const QString &TcpClient::getCurpath()
{
    return m_curPath;
}

void TcpClient::changeCurpath(const QString &dirName, bool flag)
{
    if(flag){
        m_curPath += QString("/%1").arg(dirName);
    }
    else{
        int idx = m_curPath.lastIndexOf("/");
        m_curPath.remove(idx, m_curPath.size() - idx);
    }
}

void TcpClient::setDownloadStatus(bool status)
{
    m_isdownLoad = status;
}

void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接成功");
}

void TcpClient::rec_Msg()
{

    if(!m_isdownLoad){
        qDebug()<<m_tcpSocket.bytesAvailable();
        uint uiPDULen = 0;
        m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));      //先获取PDU的前4个字节，内容为PDU的实际总大小
        uint uiMsgLen = uiPDULen-sizeof(PDU);           //减去PDU前面固定变量的大小，得到实际消息大小
        PDU *pdu = mk_pdu(uiMsgLen);
        m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen - sizeof(uint));//接收剩余数据
        switch (pdu->uiMsgType) {
        case ENUM_MSG_TYPE_REGISTER_RESPOND:{

            if(strcmp(pdu->caData, REG_OK) == 0){
                QMessageBox::information(this, "注册", "注册成功");
            }
            else if(strcmp(pdu->caData, REG_FAIL) == 0){
                QMessageBox::warning(this, "注册", "注册失败");
            }
            break;

        }
        case ENUM_MSG_TYPE_LOGIN_IN_RESPOND:{
            if(strcmp(pdu->caData, LOG_OK) == 0){
                QMessageBox::information(this, "登陆", "登陆成功");
                char path[pdu->uiMsgLen];
                memset(path, '\0', pdu->uiMsgLen);
                memcpy(path, pdu->caMsg, pdu->uiMsgLen);
                m_curPath = QString(path);                    //设置用户操作路径
                OperateWidget::getInstance().show();
                this->hide();
            }
            else if(strcmp(pdu->caData, LOG_FAIL) == 0){
                QMessageBox::warning(this, "登陆", "登陆失败");
            }
            break;
        }
        case ENUM_MSG_TYPE_QUERY_USER_RESPOND:{
            OperateWidget::getInstance().getFriendWidget()->handleOnlineQueryRes(pdu);
            break;
        }
        case ENUM_MSG_TYPE_REFRESH_DIR_RESPOND:{
            OperateWidget::getInstance().getMainWidget()->handleRefreshDir(pdu);
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:{
            OperateWidget::getInstance().getMainWidget()->handleCreateDir(pdu);
            break;
        }
        case ENUM_MSG_TYPE_RENAME_DIR_RESPOND:{
            OperateWidget::getInstance().getMainWidget()->handleRename(pdu);
            break;
        }
        case ENUM_MSG_TYPE_REMOVE_RESPOND:{
            OperateWidget::getInstance().getMainWidget()->handleRemove(pdu);
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:{
            OperateWidget::getInstance().getMainWidget()->handleUpLoad(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:{
            OperateWidget::getInstance().getMainWidget()->handleDownLoad(pdu);
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_MESSAGE_RESPOND:{
            OperateWidget::getInstance().getFriendWidget()->handlePrivateMsgSendRes(pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
            OperateWidget::getInstance().getFriendWidget()->handleAddFriendRes(pdu);
            break;
        }
        case ENUM_MSG_TYPE_QUERY_ALL_FRIEND_RESPOND:{
            OperateWidget::getInstance().getFriendWidget()->handleFriendInfoRes(pdu);
            break;
        }
        case ENUM_MSG_TYPE_QUERY_ALL_ADD_FRIEND_RESPOND:{
            OperateWidget::getInstance().getFriendWidget()->handleFriendQuestRes(pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_COMMON_RESPOND:{
            OperateWidget::getInstance().getFriendWidget()->handleAddFriendAcceptAndRefuseRes(pdu);
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_MESSAGE_TRANSPOND:{
            OperateWidget::getInstance().getFriendWidget()->handleTranspondMessage(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
            OperateWidget::getInstance().getMainWidget()->handleShareFile(pdu);
            break;
        }
        case ENUM_MSG_TYPE_QUERY_ALL_SHARE_FILE_RESPOND:{
            OperateWidget::getInstance().getFriendWidget()->handleShareFileInfoRes(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_REFUSE_RESPOND:{
            OperateWidget::getInstance().getFriendWidget()->handleRefuseShareRes(pdu);
            break;
        }
        default:{
            qDebug()<<"未知消息";
            break;
        }
        }
        free(pdu);
        pdu = nullptr;
    }
    else{
        QByteArray buffer = m_tcpSocket.readAll();
        MainWidget* pMainWidget = OperateWidget::getInstance().getMainWidget();
        pMainWidget->m_File.write(buffer);
        pMainWidget->m_iRevcdSize += buffer.size();
        qDebug()<<pMainWidget->m_iTotalSize <<"\\"<<pMainWidget->m_iRevcdSize;
        if(pMainWidget->m_iTotalSize == pMainWidget->m_iRevcdSize){
            pMainWidget->m_File.close();
            pMainWidget->m_iTotalSize = 0;
            pMainWidget->m_iRevcdSize = 0;
            setDownloadStatus(false);
            pMainWidget->sendSnkMessage(QString("下载成功，保存到\n%1").arg(pMainWidget->getDownLoadPath()));
            OperateWidget::getInstance().getFriendWidget()->sendSnkMessage(QStringLiteral("下载成功，已保存到默认下载位置"));  //这是下载分享文件的提示
        }
        else if(pMainWidget->m_iTotalSize < pMainWidget->m_iRevcdSize){
            pMainWidget->m_File.close();
            pMainWidget->m_iTotalSize = 0;
            pMainWidget->m_iRevcdSize = 0;
            setDownloadStatus(false);
            pMainWidget->sendSnkMessage(QStringLiteral("下载失败:文件异常"));
        }

    }
}


void TcpClient::on_login_in_btn_clicked()
{
    QString user_name = ui->name_line_edit->text();
    QString user_pwd = ui->pwd_line_edit->text();
    m_name = user_name;
    if(!user_pwd.isEmpty() && !user_name.isEmpty()){
        PDU *pdu = mk_pdu(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_IN_REQUEST;        //消息类型设置为请求登陆
        strncpy(pdu->caData, user_name.toStdString().c_str(), 128); //ca_data前128个字节放用户名
        strncpy(pdu->caData + 128,user_pwd.toStdString().c_str(), 128);//后128字节放密码
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);              //发送消息
        free(pdu);
        pdu = nullptr;
    }
    else{
        QMessageBox::critical(this,"登陆","用户名或密码不能为空");
    }
}

void TcpClient::on_reg_btn_clicked()
{
    QString user_name = ui->name_line_edit->text();
    QString user_pwd = ui->pwd_line_edit->text();
    if(!user_pwd.isEmpty() && !user_name.isEmpty()){
        PDU *pdu = mk_pdu(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_REQUEST;        //消息类型设置为请求注册
        strncpy(pdu->caData,user_name.toStdString().c_str(), 128); //ca_data前128个字节放用户名
        strncpy(pdu->caData + 128,user_pwd.toStdString().c_str(), 128);//后128字节放密码
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);              //发送消息
        free(pdu);
        pdu = nullptr;
    }
    else{
        QMessageBox::critical(this,"注册","用户名或密码不能为空");
    }
}

void TcpClient::on_login_out_btn_clicked()
{

}

