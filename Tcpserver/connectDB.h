#ifndef CONNECTDB_H
#define CONNECTDB_H
#include "protocol.h"
#include <QSql>
#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QPair>


/**************************************************************/
//                        数据库操作类
/**************************************************************/
class connectDB : public QObject{
    Q_OBJECT                   //给自己的类加QObject宏的时候，记得再执行一遍qmake
public:

    explicit connectDB(QObject *parent = 0);

    connectDB(const connectDB &c) = delete;

    ~connectDB();
    //数据库初始化
    bool init();
    //获得单例对象
    static connectDB& getInstance();

    bool handleReg(const QString& name, const QString& pwd);      //处理注册请求
    bool handleLog(const QString& name, const QString& pwd);      //处理登陆请求
    bool handleOffline(const QString& name);                   //处理注销请求
    int  handleQueryUser(const QString& name);                 //处理查询用户请求
    bool handlePriMsg(const QString& sender, const QString& receiver, const QString& msg);   //处理私聊消息
    bool handleAddFriendQuest(const QString& sender, const QString& receiver);
    bool handleAddFriendAccept(const QString& sender, const QString& receiver);
    bool handleAddFriendRefuse(const QString& sender, const QString& receiver);
    bool handleShareFile(const QString& sender, const QString& receiver, const QString& path, const QString& fileName);
    bool handleUpdateShareStatus(const QString& userName, const QString& fileName);
    QList<QString> handleQueryAddFriendQuest(const QString& userName);
    QList<QPair<QString, QString>> handleQueryShareFile(const QString& userName);
    QList<FriendInfo*> handleQueryAllFriends(const QString& userName);


private:
    QSqlDatabase db;
};
#endif // CONNECTDB_H
