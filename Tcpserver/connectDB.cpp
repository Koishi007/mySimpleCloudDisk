#include"connectDB.h"

connectDB::connectDB(QObject *parent) : QObject(parent){
    db = QSqlDatabase::addDatabase("QMYSQL");             //ODBC驱动
    qDebug()<<"QMYSQL driver?"<<db.isValid();
}

connectDB::~connectDB()
{
    db.close();
}

bool connectDB::init()
{
    QString dsn = QString::fromLocal8Bit("cloud_disk");     //数据源名称
    db.setHostName("127.0.0.1");                        //选择本地主机，127.0.1.1
    db.setDatabaseName(dsn);                            //设置数据源名称
    db.setUserName("root");                         //登录用户
    db.setPassword("20020202");                         //密码
    if(!db.open())                                      //打开数据库
    {
        qDebug()<<db.lastError().text();
        QMessageBox::critical(0, QObject::tr("Database error"), db.lastError().text());
        return false;                                   //打开失败
    }
    else
    {
        qDebug()<<"database open success!";
    }return true;
}


connectDB &connectDB::getInstance()
{
    static connectDB Instance;
    return Instance;
}

bool connectDB::handleReg(const QString& name, const QString& pwd)
{
    if(name.isEmpty() || pwd.isEmpty())return false;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("insert into userinfo (user_name, pwd) values(:name, :pwd)"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":pwd"), pwd);
    return query.exec();
}

bool connectDB::handleLog(const QString& name, const QString& pwd)
{
    if(name.isEmpty() || pwd.isEmpty())return false;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("select * from userinfo where user_name = :name and pwd = :pwd"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":pwd"), pwd);
    query.exec();
    if(query.first()){
        query.prepare(QStringLiteral("update userinfo set online = 1 where user_name = :name"));    //用bindvalue ，字符串可以不用引号
        query.bindValue(QStringLiteral(":name"),name);
        return query.exec();
    }else{
        return false;
    }
}

bool connectDB::handleOffline(const QString& name)
{
    if(name == nullptr){
        return false;
    }
    QSqlQuery query(db);
    query.prepare(QStringLiteral("update userinfo set online = 0 where user_name = :name"));    //用bindvalue ，字符串可以不用引号
    query.bindValue(QStringLiteral(":name"),name);
    return query.exec();
}

int connectDB::handleQueryUser(const QString& name)
{
    QSqlQuery query(db);
    query.prepare(QStringLiteral("select online from userinfo where user_name = :name"));
    query.bindValue(QStringLiteral(":name"), name);
    query.exec();
    if(query.first()){
        int t_online = query.value(0).toInt();
        return t_online;
    }
    return -1;
}

bool connectDB::handlePriMsg(const QString& sender, const QString& receiver, const QString& msg)
{
    QSqlQuery query(db);
    query.prepare(QStringLiteral("insert into discuss_post (time, sender, receiver, msg) values(NOW(), :sender, :receiver, :msg)"));
    query.bindValue(QStringLiteral(":sender"), sender);
    query.bindValue(QStringLiteral(":receiver"), receiver);
    query.bindValue(QStringLiteral(":msg"), msg);
    return query.exec();
}

bool connectDB::handleAddFriendQuest(const QString& sender, const QString& receiver)
{
    QSqlQuery query(db);
    query.prepare(QStringLiteral("insert into add_friend_info (sender, receiver, time) values(:sender, :receiver, NOW())"));
    query.bindValue(QStringLiteral(":sender"), sender);
    query.bindValue(QStringLiteral(":receiver"), receiver);
    return query.exec();
}

bool connectDB::handleAddFriendAccept(const QString& sender, const QString& receiver)
{
    QSqlQuery query(db);
    query.prepare(QStringLiteral("insert into friendinfo (User1, User2) values(:sender, :receiver)"));      //更新朋友信息表
    query.bindValue(QStringLiteral(":sender"), sender);
    query.bindValue(QStringLiteral(":receiver"), receiver);
    bool ret1 = query.exec();
    query.prepare(QStringLiteral("update add_friend_info set status = 1, accepted = 1 where sender = :sender and receiver = :receiver")); //更新好友请求表
    query.bindValue(QStringLiteral(":sender"), sender);
    query.bindValue(QStringLiteral(":receiver"), receiver);
    bool ret2 = query.exec();
    return ret1 && ret2;
}

bool connectDB::handleAddFriendRefuse(const QString& sender, const QString& receiver)
{
    QSqlQuery query(db);
    query.prepare(QStringLiteral("update add_friend_info set status = 1 where sender = :sender and receiver = :receiver"));//更新好友请求表
    query.bindValue(QStringLiteral(":sender"), sender);
    query.bindValue(QStringLiteral(":receiver"), receiver);
    return query.exec();
}

bool connectDB::handleShareFile(const QString &sender, const QString &receiver, const QString &path, const QString &fileName)
{
     QSqlQuery query(db);
     query.prepare(QStringLiteral("insert into file_share (path, file_name, sender, receiver) values(:path, :file_name, :sender, :receiver)"));
     query.bindValue(QStringLiteral(":path"), path);
     query.bindValue(QStringLiteral(":file_name"), fileName);
     query.bindValue(QStringLiteral(":sender"), sender);
     query.bindValue(QStringLiteral(":receiver"), receiver);
     return query.exec();
}

bool connectDB::handleUpdateShareStatus(const QString &userName, const QString &fileName)
{
    QSqlQuery query(db);
    query.prepare(QStringLiteral("update file_share set status = 1 where receiver = :userName and file_name = :fileName"));
    query.bindValue(QStringLiteral(":userName"), userName);
    query.bindValue(QStringLiteral(":fileName"), fileName);
    return query.exec();
}

QList<QString> connectDB::handleQueryAddFriendQuest(const QString& userName)
{
    QSqlQuery query(db);
    query.prepare(QStringLiteral("select sender from add_friend_info where receiver = :userName and status = 0"));
    query.bindValue(QStringLiteral(":userName"), userName);
    query.exec();
    QList<QString> list;
    while(query.next()){
        QString name = query.value(0).toString();
        list.append(name);
    }
    return list;
}

QList<QPair<QString, QString>> connectDB::handleQueryShareFile(const QString &userName)
{
    QSqlQuery query(db);
    QList<QPair<QString, QString>> list;
    query.prepare(QStringLiteral("select path, file_name, sender from file_share where receiver = :name and status = 0"));
    query.bindValue(QStringLiteral(":name"), userName);
    query.exec();
    while(query.next()){
        list.append(qMakePair(query.value(2).toString(), QString("%1/%2").arg(query.value(0).toString()).arg(query.value(1).toString())));
    }
    return list;
}

QList<FriendInfo *> connectDB::handleQueryAllFriends(const QString& userName)
{
    QList<FriendInfo *> list;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("select * from friendinfo where User1 = :name or User2 = :name"));
    query.bindValue(QStringLiteral(":name"), userName);
    query.exec();
    while(query.next()){
        FriendInfo *friendInfo = new FriendInfo;
        memset(friendInfo->friendName, 0, 128);
        QString name1 = query.value(1).toString();
        QString name2 = query.value(2).toString();
        if(name1 != userName){      //存储朋友关系时，没有主次之分，所以要判断一下
            memcpy(friendInfo->friendName, name1.toStdString().c_str(), name1.size() + 1);
        }
        else{
            memcpy(friendInfo->friendName, name2.toStdString().c_str(), name2.size() + 1);
        }
        list.append(friendInfo);
    }
    for(int i = 0; i < list.size(); i++){       //查询朋友们是否在线
        query.prepare(QStringLiteral("select online from userinfo where user_name = :name"));
        query.bindValue(QStringLiteral(":name"), list[i]->friendName);
        query.exec();
        if(query.next()){
           list[i]->isOnline = query.value(0).toInt();
        }
    }
    return list;
}

