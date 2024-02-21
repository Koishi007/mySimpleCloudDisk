#include "friendwidget.h"
#include "ui_friendwidget.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>
#include <QDateTime>


FriendWidget::FriendWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendWidget),
    m_AddFriendMessageTable(new QTableWidget),
    m_ShareMessageTable(new QTableWidget),
    m_DefaultMessageBox(new QTextBrowser),
    m_FileShareInfoWidget(new FileShareInfoWidget),
    m_Snkbar(new QtMaterialSnackbar),
    m_Drawer(new QtMaterialDrawer)
{
    ui->setupUi(this);

    ui->friend_table->setColumnCount(3);
    ui->friend_table->horizontalHeader()->setVisible(false);
    ui->friend_table->verticalHeader()->setVisible(false);
    ui->friend_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //先自适应宽度
    ui->friend_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    ui->friend_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->friend_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->friend_table->setShowGrid(false);
    ui->friend_table->setStyleSheet("QTableWidget {selection-background-color: rgb(185, 205, 241);}");



    m_Snkbar->setParent(this);

    m_Drawer->setParent(this);
    m_Drawer->setClickOutsideToClose(true);
    m_Drawer->setOverlayMode(true);
    m_Drawer->setDrawerWidth(280);
    m_Drawer->setStyleSheet(QStringLiteral("QTableWidget {selection-background-color: rgb(185, 205, 241);}"
                            "QScrollBar{ background: white; width:5px}"
                            "QScrollBar::handle:vertical{ background-color: #86dbcb; min-height: 60px ;width:30px }"));
    QVBoxLayout *drawerLayout = new QVBoxLayout;
    drawerLayout->setSpacing(5);
    m_Drawer->setDrawerLayout(drawerLayout);

    //好友请求表
    m_AddFriendMessageLable = new QLabel(QStringLiteral("好友请求"));
    m_AddFriendMessageLable->setFont(QFont(QStringLiteral("微软雅黑"), 10, QFont::Medium));
    drawerLayout->addWidget(m_AddFriendMessageLable);
    m_AddFriendMessageTable->setColumnCount(3);
    m_AddFriendMessageTable->horizontalHeader()->setVisible(false);
    m_AddFriendMessageTable->verticalHeader()->setVisible(false);
    m_AddFriendMessageTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //先自适应宽度
    m_AddFriendMessageTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    m_AddFriendMessageTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_AddFriendMessageTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_AddFriendMessageTable->setShowGrid(false);
    drawerLayout->addWidget(m_AddFriendMessageTable);

    //分享通知表
    m_ShareMessageLable = new QLabel(QStringLiteral("文件分享"));
    m_ShareMessageLable->setFont(QFont(QStringLiteral("微软雅黑"), 10, QFont::Medium));
    drawerLayout->addWidget(m_ShareMessageLable);
    m_ShareMessageTable->setColumnCount(2);
    m_ShareMessageTable->horizontalHeader()->setVisible(false);
    m_ShareMessageTable->verticalHeader()->setVisible(false);
    m_ShareMessageTable->setColumnWidth(0,210);
    m_ShareMessageTable->setColumnWidth(1,50);
    m_ShareMessageTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ShareMessageTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ShareMessageTable->setShowGrid(false);
    drawerLayout->addWidget(m_ShareMessageTable);

    //默认聊天框，接受方初始化为自己
    m_DefaultMessageBox->setReadOnly(true);
    m_DefaultMessageBox->setFont(QFont(QStringLiteral("微软雅黑"), 15, QFont::Medium));
    m_DefaultMessageBox->setLineWrapMode(QTextBrowser::WidgetWidth);
    ui->message_stack_widget->addWidget(m_DefaultMessageBox);
    ui->message_stack_widget->setCurrentWidget(m_DefaultMessageBox);
    m_ReceiverName = TcpClient::getInstance().getUserName();

    connect(ui->friend_table, &QTableWidget::itemDoubleClicked,[=](){
        int curRow = ui->friend_table->currentRow();
        if(curRow < 0)return;
        QTableWidgetItem *item = ui->friend_table->item(curRow, 1);
        m_ReceiverName = item->text();
        ui->message_stack_widget->setCurrentWidget(m_MessageBoxMap.value(m_ReceiverName));
        sendSnkMessage(QStringLiteral("私聊对象变更"));
    });

    connect(ui->message_line, &QLineEdit::returnPressed, this, &FriendWidget::on_message_btn_clicked);

    connect(this, &FriendWidget::sendFileShareInfo, m_FileShareInfoWidget, &FileShareInfoWidget::initFileShareInfo);

    initFriendTable();

}

FriendWidget::~FriendWidget()
{
    delete ui;
    delete m_AddFriendMessageTable;
    delete m_AddFriendMessageLable;
    delete m_ShareMessageTable;
    delete m_ShareMessageLable;
    delete m_DefaultMessageBox;
    delete m_FileShareInfoWidget;
    foreach(auto it , m_MessageBoxMap){
        delete it;
    }
    foreach(auto it, m_ShareFileInfoMap){
        free(it);
    }
    qDebug()<<"FriW析构";
}


void FriendWidget::handleOnlineQueryRes(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    ui->friend_table->setRowCount(1);
    ui->friend_table->setColumnCount(3);
    uint data = 0;
    if(strcmp(pdu->caData + 32, QUERY_OK) == 0){
        memcpy(&data, pdu->caData, sizeof(data));
    }
    else if(strcmp(pdu->caData + 32, QUERY_FAIL) == 0){
        sendSnkMessage(QStringLiteral("用户不存在"));
        return;
    }
    QString name = ui->friend_query_line->text();
    QString online = data ? QStringLiteral("在线") : QStringLiteral("不在线");
    ui->friend_table->setItem(0, 0, new QTableWidgetItem(name));
    ui->friend_table->setItem(0, 1, new QTableWidgetItem(online));
    QPushButton *btn = new QPushButton(QStringLiteral("加好友"));
    ui->friend_table->setCellWidget(0, 2, btn);
    connect(btn, &QPushButton::pressed, this, &FriendWidget::addFriendQuest);
}

void FriendWidget::handlePrivateMsgSendRes(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, SEND_PRIVATE_MSG_OK) == 0){
        sendSnkMessage(QStringLiteral("发送成功"));
    }
    else if(strcmp(pdu->caData, SEND_PRIVATE_MSG_FAIL) == 0){
        sendSnkMessage(QStringLiteral("发送失败"));
    }
}

void FriendWidget::handleAddFriendRes(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, ADD_FRIEND_QUEST_OK) == 0){
        sendSnkMessage(QStringLiteral("好友请求已发送"));
    }
    else if(strcmp(pdu->caData, ADD_FRIEND_QUEST_FAIL) == 0){
        sendSnkMessage(QStringLiteral("好友请求发送失败"));
    }
}

void FriendWidget::handleFriendInfoRes(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    FriendInfo* friendInfoP = nullptr;
    int count = pdu->uiMsgLen / sizeof(FriendInfo);
    ui->friend_table->setColumnCount(3);
    ui->friend_table->setRowCount(count);
    for(int i = 0; i < count; i++){
        friendInfoP = (FriendInfo*)pdu->caMsg + i;
        QTableWidgetItem *icon_item = new QTableWidgetItem;
        QIcon icon(friendInfoP->isOnline ? QStringLiteral(":/icon/resource/online.png") : QStringLiteral(":/icon/resource/offline.png"));
        icon_item->setIcon(icon);
        QString online = friendInfoP->isOnline ? QStringLiteral("在线") : QStringLiteral("不在线");
        ui->friend_table->setItem(i, 0, icon_item);
        ui->friend_table->setItem(i, 1, new QTableWidgetItem(friendInfoP->friendName));
        ui->friend_table->setItem(i, 2, new QTableWidgetItem(online));
        m_FriendSet.insert(friendInfoP->friendName);
        if(m_MessageBoxMap.contains(friendInfoP->friendName))continue;      //与set不同，对map插入重复键会覆盖原有数据，所以得判断一下
        m_MessageBoxMap.insert(friendInfoP->friendName, new QTextBrowser);
        m_MessageBoxMap.value(friendInfoP->friendName)->setReadOnly(true);
        m_MessageBoxMap.value(friendInfoP->friendName)->setFont(QFont(QStringLiteral("微软雅黑"), 15, QFont::Medium));
        m_MessageBoxMap.value(friendInfoP->friendName)->setLineWrapMode(QTextBrowser::WidgetWidth);
        ui->message_stack_widget->addWidget(m_MessageBoxMap.value(friendInfoP->friendName));
    }

}

void FriendWidget::handleFriendQuestRes(PDU *pdu)
{
    initShareFileTable();   //两个表格的刷新，文件分享表请求在好友请求表获得回复pdu再发送，防止黏包，以后有机会的话改成请求队列形式
    if(pdu == nullptr){
        return;
    }
    int count = pdu->uiMsgLen/128;
    m_AddFriendMessageTable->setRowCount(count);
    for(int i = 0; i < count; i++){
        char userName[128];
        memset(userName, 0, 128);
        memcpy(userName, (char*)pdu->caMsg + (i*128), 128);
        QPushButton *acceptBtn = new QPushButton(QStringLiteral("接受"));
        QPushButton *refuseBtn = new QPushButton(QStringLiteral("拒绝"));
        m_AddFriendMessageTable->setItem(i, 0, new QTableWidgetItem(userName));
        m_AddFriendMessageTable->setCellWidget(i, 1, acceptBtn);
        m_AddFriendMessageTable->setCellWidget(i, 2, refuseBtn);
        connect(acceptBtn, &QPushButton::pressed, this, &FriendWidget::acceptAddFriend);
        connect(refuseBtn, &QPushButton::pressed, this, &FriendWidget::refuseAddFriend);
    }
}

void FriendWidget::handleShareFileInfoRes(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    int count =  pdu->uiMsgLen / sizeof(ShareFileInfo);
    m_ShareMessageTable->setRowCount(count);
    ShareFileInfo* shareFileInfoP = nullptr;
    for(int i = 0; i < count; i++){
        shareFileInfoP = (ShareFileInfo*)pdu->caMsg + i;       
        if(!m_ShareFileInfoMap.contains(shareFileInfoP->fileName)){
            ShareFileInfo* t_info = (ShareFileInfo*)malloc(sizeof(ShareFileInfo));
            memcpy(t_info, shareFileInfoP, sizeof(ShareFileInfo));
            m_ShareFileInfoMap.insert(shareFileInfoP->fileName, t_info);        
        }
        else{
            continue;
        }
    }
    int row = 0;
    foreach(auto it, m_ShareFileInfoMap){       //填充表格
        QPushButton *viewBtn = new QPushButton(QStringLiteral("查看"));
        m_ShareMessageTable->setItem(row, 0, new QTableWidgetItem(it->fileName));
        m_ShareMessageTable->setCellWidget(row, 1, viewBtn);
        connect(viewBtn, &QPushButton::pressed, [=](){
            QTableWidgetItem *item = m_ShareMessageTable->item(m_ShareMessageTable->currentRow(), 0);
            emit sendFileShareInfo(item->text(), m_ShareFileInfoMap.value(item->text())->sender, m_ShareFileInfoMap.value(item->text())->fileSize);
            m_FileShareInfoWidget->show();
        });
        row++;
    }
}

void FriendWidget::handleAddFriendAcceptAndRefuseRes(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, ADD_FRIEND_ACCEPT_OK) == 0){
        sendSnkMessage(QStringLiteral("好友添加成功"));
    }
    else if(strcmp(pdu->caData, ADD_FRIEND_ACCEPT_FAIL) == 0){
        sendSnkMessage(QStringLiteral("出错了，请稍后再试"));
    }
    else if(strcmp(pdu->caData, ADD_FRIEND_REFUSE_OK) == 0){
        sendSnkMessage(QStringLiteral("您已成功拒绝"));
    }
    else if(strcmp(pdu->caData, ADD_FRIEND_REFUSE_FAIL) == 0){
        sendSnkMessage(QStringLiteral("出错了，请稍后再试"));
    }
    m_AddFriendMessageTable->clearContents();
    initAddFriendTable();
}

void FriendWidget::handleTranspondMessage(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    char sender[128];
    char c_msg[pdu->uiMsgLen];
    memset(sender, 0, 128);
    memset(c_msg, 0, pdu->uiMsgLen);
    strncpy(sender, pdu->caData, 128);
    memcpy(c_msg, pdu->caMsg, pdu->uiMsgLen);
    QTextBrowser *curMessageBox = m_MessageBoxMap.value(sender);
    QDateTime t_DateTime =QDateTime::currentDateTime();
    QString currentTimeStr =t_DateTime.toString(QStringLiteral("yyyy.MM.dd hh:mm:ss"));
    curMessageBox->moveCursor(QTextCursor::End);
    curMessageBox->insertHtml(QString("<i style=\"color:blue\">%1</i>").arg(sender));
    curMessageBox->insertHtml(QString("<b style=\"color:black\">::%1<br></b>").arg(currentTimeStr));
    curMessageBox->insertHtml(QString("<p>%1<br></p>").arg(c_msg));
    curMessageBox->moveCursor(QTextCursor::End);
}

void FriendWidget::handleRefuseShareRes(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, SHARE_REFUSE_OK) == 0){
        sendSnkMessage(QStringLiteral("拒绝成功"));
        char fileName[128];         //清除掉拒绝的分享文件请求
        memcpy(pdu->caData + 128, fileName, 128);
        free(m_ShareFileInfoMap.value(fileName));
        m_ShareFileInfoMap.remove(fileName);
        m_ShareMessageTable->clearContents();
        initShareFileTable();
        m_FileShareInfoWidget->hide();
    }
    else if(strcmp(pdu->caData, SHARE_REFUSE_FAIL) == 0){
        sendSnkMessage(QStringLiteral("拒绝失败"));
    }
}

void FriendWidget::sendSnkMessage(const QString &msg)
{
    m_Snkbar->addMessage(msg);
}

const QSet<QString> &FriendWidget::getFriendSet()
{
    return m_FriendSet;
}

void FriendWidget::on_friend_btn_clicked()
{
    PDU* pdu = mk_pdu(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_QUERY_USER_REQUEST;
    QString name = ui->friend_query_line->text();
    if(name.isEmpty()){
        sendSnkMessage(QStringLiteral("请输入查询用户名"));
        return;
    }
    strncpy(pdu->caData, name.toStdString().c_str(), ui->friend_query_line->text().toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
    ui->friend_table->clearContents();
}

void FriendWidget::on_message_btn_clicked()
{

    QString msg = ui->message_line->text();
    if(msg.isEmpty()){
        return;
    }
    QTextBrowser *curMessageBox = m_DefaultMessageBox;
    if(m_ReceiverName != TcpClient::getInstance().getUserName()){       //如果发送方是自己就不向服务器请求
        qint64 msgSize = msg.toUtf8().size() + 1;
        PDU* pdu = mk_pdu(msgSize);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_MESSAGE_REQUEST;
        QString senderName = TcpClient::getInstance().getUserName();
        memcpy(pdu->caMsg, msg.toStdString().c_str(), msgSize);
        memcpy(pdu->caData, m_ReceiverName.toStdString().c_str(), m_ReceiverName.toUtf8().size() + 1);
        memcpy(pdu->caData + 128, senderName.toStdString().c_str(), senderName.toUtf8().size() + 1);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = nullptr;
        curMessageBox = m_MessageBoxMap.value(m_ReceiverName);
    }

    QDateTime t_DateTime =QDateTime::currentDateTime();
    QString currentTimeStr =t_DateTime.toString(QStringLiteral("yyyy.MM.dd hh:mm:ss"));
    QString userName = TcpClient::getInstance().getUserName();
    curMessageBox->moveCursor(QTextCursor::End);
    curMessageBox->insertHtml(QString("<i style=\"color:green\">%1</i>").arg(userName));
    curMessageBox->insertHtml(QString("<b style=\"color:black\">::%1<br></b>").arg(currentTimeStr));
    curMessageBox->insertHtml(QString("<p>%1<br></p>").arg(msg));
    curMessageBox->moveCursor(QTextCursor::End);
}

void FriendWidget::on_refresh_btn_clicked()
{
    ui->friend_table->clearContents();
    initFriendTable();
}

void FriendWidget::addFriendQuest()
{
    int curRow = ui->friend_table->currentRow();
    QTableWidgetItem *item = ui->friend_table->item(curRow, 0);
    QString friendName = item->text();
    if(m_FriendSet.contains(friendName)){
        sendSnkMessage(QStringLiteral("你们已经是好友了哦"));
        return;
    }
    else if(friendName == TcpClient::getInstance().getUserName()){
        sendSnkMessage(QStringLiteral("每个人都是自己的好朋友"));
        return;
    }
    QString userName = TcpClient::getInstance().getUserName();
    PDU* pdu = mk_pdu(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, userName.toStdString().c_str(), userName.toUtf8().size() + 1);
    memcpy(pdu->caData + 128, friendName.toStdString().c_str(), friendName.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void FriendWidget::initFriendTable()
{
    QString userName = TcpClient::getInstance().getUserName();
    PDU* pdu = mk_pdu(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_QUERY_ALL_FRIEND_REQUEST;
    memcpy(pdu->caData, userName.toStdString().c_str(), userName.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void FriendWidget::initAddFriendTable()
{
    QString userName = TcpClient::getInstance().getUserName();
    PDU* pdu = mk_pdu(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_QUERY_ALL_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, userName.toStdString().c_str(), userName.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void FriendWidget::initShareFileTable()
{
    PDU* pdu = mk_pdu(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_QUERY_ALL_SHARE_FILE_REQUEST;
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void FriendWidget::acceptAddFriend()
{
    int curRow = m_AddFriendMessageTable->currentRow();
    QTableWidgetItem *item = m_AddFriendMessageTable->item(curRow, 0);
    QString sender = item->text();
    QString receiver = TcpClient::getInstance().getUserName();
    PDU* pdu = mk_pdu(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_ACCEPT;
    memcpy(pdu->caData, sender.toStdString().c_str(), sender.toUtf8().size() + 1);
    memcpy(pdu->caData + 128, receiver.toStdString().c_str(), receiver.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void FriendWidget::refuseAddFriend()
{
    int curRow = m_AddFriendMessageTable->currentRow();
    QTableWidgetItem *item = m_AddFriendMessageTable->item(curRow, 0);
    QString sender = item->text();
    QString receiver = TcpClient::getInstance().getUserName();
    PDU* pdu = mk_pdu(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
    memcpy(pdu->caData, sender.toStdString().c_str(), sender.toUtf8().size() + 1);
    memcpy(pdu->caData + 128, receiver.toStdString().c_str(), receiver.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}



void FriendWidget::on_show_message_drawer_btn_clicked()
{
    m_AddFriendMessageTable->clearContents();
    m_ShareMessageTable->clearContents();
    initAddFriendTable();
    m_Drawer->openDrawer();
}

void FriendWidget::on_myself_msgbox_btn_clicked()
{
    ui->message_stack_widget->setCurrentWidget(m_DefaultMessageBox);
    m_ReceiverName = TcpClient::getInstance().getUserName();
}
