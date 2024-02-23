#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H

#include <QWidget>
#include "qtmaterialappbar.h"
#include "qtmaterialsnackbar.h"
#include "qtmaterialdrawer.h"
#include "fileShareInfoWidget.h"
#include "protocol.h"
#include <QSet>
#include <QTableWidget>
#include <QLabel>
#include <QTextBrowser>

namespace Ui {
class FriendWidget;
}

class FriendWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FriendWidget(QWidget *parent = nullptr);
    ~FriendWidget();
    void handleOnlineQueryRes(PDU *pdu);
    void handlePrivateMsgSendRes(PDU *pdu);
    void handleAddFriendRes(PDU* pdu);
    void handlePrivateMsgRes(PDU* pdu);
    void handleFriendInfoRes(PDU* pdu);
    void handleFriendQuestRes(PDU* pdu);
    void handleShareFileInfoRes(PDU* pdu);
    void handleAddFriendAcceptAndRefuseRes(PDU* pdu);
    void handleTranspondMessage(PDU* pdu);
    void handleRefuseShareRes(PDU* pdu);
    void sendSnkMessage(const QString& msg);    //弹出提示条

    const QSet<QString>& getFriendSet();

signals:
    void sendFileShareInfo(QString fileName, QString sender, qint64 fileSize);
private slots:
    void on_friend_btn_clicked();
    void on_message_btn_clicked();
    void on_show_message_drawer_btn_clicked();
    void on_refresh_btn_clicked();
    void on_myself_msgbox_btn_clicked();

    void addFriendQuest();
    void initFriendTable();
    void initAddFriendTable();
    void initShareFileTable();
    void acceptAddFriend();
    void refuseAddFriend();


private:
    Ui::FriendWidget *ui;
    QString m_ReceiverName;           //私聊消息接收对象名字
    QSet<QString> m_FriendSet;          //朋友列表
    QMap<QString, ShareFileInfo*> m_ShareFileInfoMap;   //分享文件的信息
    QMap<QString, QTextBrowser*> m_MessageBoxMap;       //聊天框列表
    QTableWidget* m_AddFriendMessageTable;
    QTableWidget* m_ShareMessageTable;
    QLabel* m_AddFriendMessageLable;
    QLabel* m_ShareMessageLable;
    QTextBrowser* m_DefaultMessageBox;
    FileShareInfoWidget* m_FileShareInfoWidget;

    QtMaterialSnackbar *m_Snkbar;
    QtMaterialDrawer *m_Drawer;

};

#endif // FRIENDWIDGET_H
