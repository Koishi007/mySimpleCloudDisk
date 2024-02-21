#include "mytcpsocket.h"
#include "mytcpserver.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QStringList>
#include <QDateTime>

//有关接收sender信息的其实是没必要的，类成员已经已经有userName了，但是懒得改了，写了就写了

MyTcpSocket::MyTcpSocket():
    m_timer(new QTimer),
    m_isUpload(false)
{
    connect(this, &MyTcpSocket::readyRead, this, &MyTcpSocket::rec_Msg);   //数据缓冲区接收到数据就会发送readyread
    connect(this, &MyTcpSocket::disconnected, this, &MyTcpSocket::clientOffline);  //断开连接就会发出disconnected
    connect(m_timer, &QTimer::timeout, this, &MyTcpSocket::sendFileToClient);
    connect(this, &MyTcpSocket::onLine, MyTcpServer::getInstance().getTcpServerP(), &MyTcpServer::addTcpSocket);
    connect(this, &MyTcpSocket::offLine, MyTcpServer::getInstance().getTcpServerP(), &MyTcpServer::deleteTcpSocket);
}

MyTcpSocket::~MyTcpSocket()
{
    delete m_timer;
}

qint64 MyTcpSocket::calTotalDirSize(const QString &path, const QString &t_strFileName)
{
    qint64 totalSize = 0;
    QDir t_dir(QString("%1/%2").arg(path).arg(t_strFileName));
    QFileInfoList t_fileInfoList = t_dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for(int i = 0; i < t_fileInfoList.size(); i++){
        if(t_fileInfoList[i].isFile()){
            totalSize += t_fileInfoList[i].size();
        }
        else if(t_fileInfoList[i].isDir()){
            QString t_path(path);
            t_path.append("/");
            t_path.append(t_strFileName);
            qDebug()<<t_path;
            totalSize += calTotalDirSize(t_path, t_fileInfoList[i].fileName());
        }
    }
    return totalSize;
}

void MyTcpSocket::rec_Msg()
{
    //首先判断是否是接收文件的状态
    if(!m_isUpload){
        qDebug()<<this->bytesAvailable();
        uint uiPDULen = 0;
        this->read((char*)&uiPDULen,sizeof(uint));      //先获取PDU的前4个字节，内容为PDU的实际总大小
        uint uiMsgLen = uiPDULen-sizeof(PDU);           //减去PDU前面固定变量的大小，得到实际消息大小
        PDU *pdu = mk_pdu(uiMsgLen);                    //mk_pdu的参数是额外空间，就是弹性结构体
        this->read((char*)pdu+sizeof(uint),uiPDULen - sizeof(uint));//接收剩余数据
        switch (pdu->uiMsgType) {
        case ENUM_MSG_TYPE_REGISTER_REQUEST:{
            char c_name[128];
            char c_pwd[128];
            memset(c_name, '\0', 128);
            memset(c_pwd, '\0', 128);
            strncpy(c_name, pdu->caData, 128);
            strncpy(c_pwd, pdu->caData + 128, 128);
            bool ret = connectDB::getInstance().handleReg(c_name, c_pwd);   //注册事件
            PDU* respdu = mk_pdu(0);                                        //回复信息
            respdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_RESPOND;
            if(ret){
                strcpy(respdu->caData, REG_OK);
                QDir dir;
                qDebug()<<dir.mkdir(QString(LOCAL_PATH) + QString("%1").arg(c_name));     //用户文件存放路径

            }
            else{
                strcpy(respdu->caData, REG_FAIL);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;

        }
        case ENUM_MSG_TYPE_LOGIN_IN_REQUEST:{
            char c_name[128];
            char c_pwd[128];
            memset(c_name, '\0', 128);
            memset(c_pwd, '\0', 128);
            strncpy(c_name, pdu->caData, 128);
            strncpy(c_pwd, pdu->caData + 128, 128);
            m_UserName = c_name;                                                //用用户名唯一标识socket
            emit onLine(m_UserName, this);
            bool ret = connectDB::getInstance().handleLog(c_name, c_pwd);   //登陆事件
            char path[sizeof(LOCAL_PATH) + 128] = LOCAL_PATH;               //用户操作路径，发送给用户
            strcat(path, c_name);                                           //拼接两个char[]
            PDU* respdu = mk_pdu(sizeof(path));                             //回复信息
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_IN_RESPOND;
            if(ret){
                strcpy(respdu->caData, LOG_OK);
                memcpy(respdu->caMsg, path, respdu->uiMsgLen);
            }
            else{
                strcpy(respdu->caData, LOG_FAIL);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_QUERY_USER_REQUEST:{
            char c_name[128];
            memset(c_name, '\0', 128);
            strncpy(c_name, pdu->caData, 128);
            int t_online = connectDB::getInstance().handleQueryUser(c_name);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_QUERY_USER_RESPOND;
            memcpy(respdu->caData, &t_online, sizeof(t_online));
            if(t_online >= 0){
                strncpy(respdu->caData + 32, QUERY_OK, 32);
                qDebug()<<"success";
            }
            else
            {
                strncpy(respdu->caData + 32, QUERY_FAIL, 32);
                qDebug()<<"fail";
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_REFRESH_DIR_REQUEST:{
            char curPath[pdu->uiMsgLen];
            memset(curPath, '\0', pdu->uiMsgLen);
            memcpy(curPath, pdu->caMsg, pdu->uiMsgLen);
            qDebug()<<"刷新文件夹:"<<curPath;
            QDir dir(curPath);
            QFileInfoList fileInfoList = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
            int fileCount = fileInfoList.size();
            PDU *respdu = mk_pdu(sizeof(FileInfo) * fileCount);
            respdu->uiMsgType = ENUM_MSG_TYPE_REFRESH_DIR_RESPOND;
            FileInfo *pFileInfo = nullptr;
            QString strFileName;
            QString strFileType;
            QDateTime DfileChangeTime;
            QString strFileChangeTime;
            qint64 fileSize;
            int isFile;
            for(int i = 0; i < fileCount; i++){
                pFileInfo = (FileInfo*)respdu->caMsg + i;
                strFileName = fileInfoList[i].fileName();
                DfileChangeTime = fileInfoList[i].lastModified();
                strFileChangeTime = DfileChangeTime.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
                fileSize = fileInfoList[i].isFile()? fileInfoList[i].size() : calTotalDirSize(QString(curPath), strFileName);
                strFileType = fileInfoList[i].suffix();
                isFile = fileInfoList[i].isFile();
                memcpy(pFileInfo->fileName, strFileName.toStdString().c_str(),
                       strFileName.toUtf8().size() + 1);    //发送可能有中文的QString数据的时候，要先先用Toutf8获得准确字节数
                memcpy(pFileInfo->fileType, strFileType.toStdString().c_str(), strFileType.size() + 1);
                memcpy(pFileInfo->fileChangeTime, strFileChangeTime.toStdString().c_str(), strFileChangeTime.size() + 1);
                memcpy(&pFileInfo->fileSize, &fileSize, sizeof(qint64));
                memcpy(&pFileInfo->isFile, &isFile, sizeof(int));
            }

            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:{
            char curPath[pdu->uiMsgLen];
            memset(curPath, '\0', pdu->uiMsgLen);
            memcpy(curPath, pdu->caMsg, pdu->uiMsgLen);
            qDebug()<<"创建文件夹:"<<curPath;
            QDir dir(curPath);
            bool flag = dir.mkdir(QString("新建文件夹(%1)").arg(dir.count() - 2));
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            if(flag){
                strncpy(respdu->caData, CREATE_OK, 32);
            }
            else{
                strncpy(respdu->caData, CREATE_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_DIR_REQUEST:{
            char curPath[pdu->uiMsgLen];
            char newName[128];
            char preName[128];
            memset(curPath, '\0', pdu->uiMsgLen);
            memset(newName, '\0', 128);
            memset(preName, '\0', 128);
            memcpy(curPath, pdu->caMsg, pdu->uiMsgLen);
            memcpy(newName, pdu->caData, 128);
            memcpy(preName, pdu->caData + 128, 128);
            qDebug()<<"(改名)新名字:"<<newName;
            qDebug()<<"(改名)旧名字:"<<preName;
            QDir dir(curPath);
            bool flag = dir.rename(preName, newName);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_DIR_RESPOND;
            if(flag){
                strncpy(respdu->caData, RENAME_OK, 32);
            }
            else{
                strncpy(respdu->caData, RENAME_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_REMOVE_DIR_REQUEST:{
            char curPath[pdu->uiMsgLen];
            char fileName[128];
            memset(curPath, '\0', pdu->uiMsgLen);
            memset(fileName, '\0', 128);
            memcpy(curPath, pdu->caMsg, pdu->uiMsgLen);
            memcpy(fileName, pdu->caData, 128);
            QDir dir(curPath);
            bool flag;
            flag = dir.rmdir(fileName);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REMOVE_RESPOND;
            if(flag){
                strncpy(respdu->caData, REMOVE_OK, 32);
            }
            else{
                strncpy(respdu->caData, REMOVE_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_REMOVE_FILE_REQUEST:{
            char curPath[pdu->uiMsgLen];
            char fileName[128];
            memset(curPath, '\0', pdu->uiMsgLen);
            memset(fileName, '\0', 128);
            memcpy(curPath, pdu->caMsg, pdu->uiMsgLen);
            memcpy(fileName, pdu->caData, 128);
            qDebug()<<"删除文件:"<<fileName;
            QDir dir(curPath);
            bool flag;
            flag = dir.remove(fileName);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REMOVE_RESPOND;
            if(flag){
                strncpy(respdu->caData, REMOVE_OK, 32);
            }
            else{
                strncpy(respdu->caData, REMOVE_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:{
            char curPath[pdu->uiMsgLen];
            char fileName[128];
            qint64 fileSize;
            memset(curPath, '\0', pdu->uiMsgLen);
            memset(fileName, '\0', 128);
            memcpy(curPath, pdu->caMsg, pdu->uiMsgLen);
            memcpy(fileName, pdu->caData, 128);
            memcpy(&fileSize, pdu->caData + 128, sizeof(qint64));
            QString strPath = QString("%1/%2").arg(curPath).arg(fileName);
            qDebug()<<"上传请求:"<<strPath;
            m_file.setFileName(strPath);
            if(m_file.open(QIODevice::WriteOnly)){      //以只写方式打开文件，若不存在则创建
                m_isUpload = true;
                m_fileTotalSize = fileSize;
                m_fileRecvedSize = 0;
            }
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:{
            char curPath[pdu->uiMsgLen];
            char fileName[128];
            memset(curPath, '\0', pdu->uiMsgLen);
            memset(fileName, '\0', 128);
            memcpy(curPath, pdu->caMsg, pdu->uiMsgLen);
            memcpy(fileName, pdu->caData, 128);
            QString strPath = QString("%1/%2").arg(curPath).arg(fileName);
            qDebug()<<"下载请求:"<<strPath;
            QFileInfo fileInfo(strPath);
            qint64 fileSize = fileInfo.size();
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            memcpy(respdu->caData, fileName, 128);
            memcpy(respdu->caData + 128, &fileSize, sizeof(qint64));
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            m_file.setFileName(strPath);
            m_file.open(QIODevice::ReadOnly);
            m_timer->start(1500);
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_MESSAGE_REQUEST:{
            char reciver[128];
            char sender[128];
            char c_msg[pdu->uiMsgLen];
            memset(reciver, '\0', 128);
            memset(sender, '\0', 128);
            memset(c_msg, '\0', pdu->uiMsgLen);
            strncpy(reciver, pdu->caData, 128);
            strncpy(sender, pdu->caData + 128, 128);
            memcpy(c_msg, pdu->caMsg, pdu->uiMsgLen);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_MESSAGE_RESPOND;
            QMap<QString, MyTcpSocket*> t_map = MyTcpServer::getInstance().getTcpSocketMap();
            if(t_map.contains(reciver)){
                connect(this, &MyTcpSocket::sendPrivateMsg, *t_map.find(reciver), &MyTcpSocket::recPrivateMsg, Qt::UniqueConnection);
                emit sendPrivateMsg(sender, c_msg);
            }
            bool ret = connectDB::getInstance().handlePriMsg(sender, reciver, c_msg);
            if(ret){
                qDebug()<<"私聊记录插入成功";
                strncpy(respdu->caData, SEND_PRIVATE_MSG_OK, 32);
            }
            else{
                qDebug()<<"私聊记录插入失败";
                strncpy(respdu->caData, SEND_PRIVATE_MSG_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
            char sender[128];
            char receiver[128];
            memset(sender, '\0', 128);
            memset(receiver, '\0', 128);
            strncpy(sender, pdu->caData, 128);
            strncpy(receiver, pdu->caData + 128, 128);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            bool ret = connectDB::getInstance().handleAddFriendQuest(sender, receiver);
            if(ret){
                strncpy(respdu->caData, ADD_FRIEND_QUEST_OK, 32);
            }
            else{
                strncpy(respdu->caData, ADD_FRIEND_QUEST_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
              break;
        }
        case ENUM_MSG_TYPE_QUERY_ALL_FRIEND_REQUEST:{
            char c_name[128];
            memset(c_name, '\0', 128);
            strncpy(c_name, pdu->caData, 128);
            QList<FriendInfo*> list = connectDB::getInstance().handleQueryAllFriends(c_name);
            FriendInfo *friendInfoP = nullptr;
            PDU* respdu = mk_pdu(sizeof(FriendInfo) * list.size());
            respdu->uiMsgType = ENUM_MSG_TYPE_QUERY_ALL_FRIEND_RESPOND;
            for(int i = 0; i < list.size(); i++){
                friendInfoP = (FriendInfo*)respdu->caMsg + i;
                memcpy(friendInfoP, list[i], sizeof(FriendInfo));
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            for(int i = 0; i < list.size(); i++){
                delete list[i];
            }
            break;
        }
        case ENUM_MSG_TYPE_QUERY_ALL_ADD_FRIEND_REQUEST:{
            char c_name[128];
            memset(c_name, '\0', 128);
            strncpy(c_name, pdu->caData, 128);
            QList<QString> list = connectDB::getInstance().handleQueryAddFriendQuest(c_name);
            PDU* respdu = mk_pdu(list.size() * 128);
            respdu->uiMsgType = ENUM_MSG_TYPE_QUERY_ALL_ADD_FRIEND_RESPOND;
            for(int i = 0; i < list.size(); i++){
                memcpy((char*)respdu->caMsg + (i * 128), list[i].toStdString().c_str(), list[i].toUtf8().size() + 1);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT:{
            char sender[128];
            char receiver[128];
            memset(sender, '\0', 128);
            memset(receiver, '\0', 128);
            strncpy(sender, pdu->caData, 128);
            strncpy(receiver, pdu->caData + 128, 128);
            bool ret = connectDB::getInstance().handleAddFriendAccept(sender, receiver);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_COMMON_RESPOND;
            if(ret){
                strncpy(respdu->caData, ADD_FRIEND_ACCEPT_OK, 32);
            }
            else{
                strncpy(respdu->caData, ADD_FRIEND_ACCEPT_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:{
            char sender[128];
            char receiver[128];
            memset(sender, '\0', 128);
            memset(receiver, '\0', 128);
            strncpy(sender, pdu->caData, 128);
            strncpy(receiver, pdu->caData + 128, 128);
            bool ret = connectDB::getInstance().handleAddFriendRefuse(sender, receiver);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_COMMON_RESPOND;
            if(ret){
                strncpy(respdu->caData, ADD_FRIEND_REFUSE_OK, 32);
            }
            else{
                strncpy(respdu->caData, ADD_FRIEND_REFUSE_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
            char curPath[pdu->uiMsgLen];
            char fileName[128];
            char receiver[128];
            memset(curPath, '\0', pdu->uiMsgLen);
            memset(fileName, '\0', 128);
            memset(receiver, '\0', 128);
            memcpy(curPath, pdu->caMsg, pdu->uiMsgLen);
            memcpy(fileName, pdu->caData, 128);
            memcpy(receiver, pdu->caData + 128, 128);
            bool ret = connectDB::getInstance().handleShareFile(m_UserName, receiver, curPath, fileName);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            if(ret){
                strncpy(respdu->caData, SHARE_FILE_OK, 32);
            }
            else{
                strncpy(respdu->caData, SHARE_FILE_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_QUERY_ALL_SHARE_FILE_REQUEST:{
            QList<QPair<QString, QString>> list = connectDB::getInstance().handleQueryShareFile(m_UserName);
            PDU *respdu = mk_pdu(list.size() * sizeof(ShareFileInfo));
            respdu->uiMsgType = ENUM_MSG_TYPE_QUERY_ALL_SHARE_FILE_RESPOND;
            ShareFileInfo *shareFileInfoP = nullptr;
            for(int i = 0; i < list.size(); i++){
                shareFileInfoP = (ShareFileInfo*)respdu->caMsg + i;
                memcpy(shareFileInfoP->sender, list[i].first.toStdString().c_str(), list[i].first.toUtf8().size() + 1);
                QFile file(list[i].second);
                int idx = list[i].second.lastIndexOf("/");
                QString fileName = list[i].second.right(list[i].second.size() - idx - 1);
                qint64 fileSize = file.size();
                memcpy(shareFileInfoP->fileName, fileName.toStdString().c_str(), fileName.toUtf8().size() + 1);
                memcpy(&shareFileInfoP->fileSize, &fileSize, sizeof(qint64));
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
            break;
        }
        case ENUM_MSG_TYPE_SHARE_REFUSE_REQUEST:{
            char fileName[128];
            memset(fileName, '\0', 128);
            memcpy(fileName, pdu->caData, 128);
            bool ret = connectDB::getInstance().handleUpdateShareStatus(m_UserName, fileName);
            PDU* respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_REFUSE_RESPOND;
            if(ret){
                strncpy(respdu->caData, SHARE_REFUSE_OK, 32);
                memcpy(respdu->caData + 128, fileName, 128);    //把拒绝分享的文件名字发送回去
            }
            else{
                strncpy(respdu->caData, SHARE_REFUSE_FAIL, 32);
            }
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
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
        PDU* respdu = nullptr;
        QByteArray buff = readAll();
        m_file.write(buff);
        m_fileRecvedSize += buff.size();
        qDebug()<<m_fileTotalSize<<"\\"<<m_fileRecvedSize;
        if(m_fileTotalSize == m_fileRecvedSize){
            m_file.close();
            m_isUpload = false;
            respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strncpy(respdu->caData, UPLOAD_OK, 32);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
        }
        else if(m_fileTotalSize < m_fileRecvedSize){
            m_file.close();
            m_isUpload = false;
            respdu = mk_pdu(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strncpy(respdu->caData, UPLOAD_FAIL, 32);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
        }

    }
}

void MyTcpSocket::clientOffline()
{
    connectDB::getInstance().handleOffline(m_UserName.toStdString().c_str());
    emit offLine(m_UserName);
}

void MyTcpSocket::sendFileToClient()
{
    m_timer->stop();
    char *pData = new char[4096];
    qint64 ret = 0;
    while(true){
        ret = m_file.read(pData, 4096);
        if(ret > 0 && ret <= 4096){
            write(pData, ret);
        }
        else if(ret == 0){
            m_file.close();
            break;
        }
        else if(ret < 0){
            qDebug()<<"发送文件内容给客户端过程中失败";
            m_file.close();
            break;

        }
    }
    delete []pData;
}

void MyTcpSocket::recPrivateMsg(QString senderName, QString msg)
{
    qDebug()<<"用户:"<<m_UserName<<"收到消息";
    qint64 msgSize = msg.toUtf8().size() + 1;
    PDU* pdu = mk_pdu(msgSize);
    pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_MESSAGE_TRANSPOND;
    memcpy(pdu->caMsg, msg.toStdString().c_str(), msgSize);
    memcpy(pdu->caData, senderName.toStdString().c_str(), senderName.toUtf8().size() + 1);
    write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;

}
