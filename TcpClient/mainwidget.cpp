#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>
#include <tcpclient.h>
#include <QIcon>
#include <QMessageBox>
#include <QFileDialog>




MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    m_File(new QFile),
    m_timer(new QTimer),  
    ui(new Ui::MainWidget),
    m_drawer(new QtMaterialDrawer),
    m_bar(new QtMaterialAppBar),
    m_snkbar(new QtMaterialSnackbar),
    m_fileTable(new QTableWidget),
    m_circularProgressWidget(new framelessWidget),
    m_shareSelectWidget(new shareSelectWidget)
{
    //ui->setupUi(this);
    this->setFixedSize(1000,600);

    QFile file(QStringLiteral(":/config/localDownLoadPath.config"));        //读取默认下载路径
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        m_defaultDownLoadPath = baData.toStdString().c_str();
        file.close();
    }
    else{
        qDebug()<<"config open fail";
    }

    //appbar字体设置
    m_menuLable = new QLabel(QStringLiteral("menu"));
    m_menuLable->setAttribute(Qt::WA_TranslucentBackground);
    m_menuLable->setForegroundRole(QPalette::Foreground);
    m_menuLable->setContentsMargins(6, 0, 0, 0);
    QPalette palette = m_menuLable->palette();
    palette.setColor(m_menuLable->foregroundRole(), Qt::white);
    m_menuLable->setPalette(palette);
    m_menuLable->setFont(QFont("微软雅黑", 18, QFont::Normal));

    m_btn = new QPushButton;         //弹出栏按钮
    m_btn->setStyleSheet(QStringLiteral("background-color: transparent; border: none;"));
    m_btn->setIcon(QIcon(QStringLiteral(":/icon/resource/menuIcon.png")));
    m_btn->setIconSize(QSize(64,64));
    m_bar->appBarLayout()->addWidget(m_btn);
    m_bar->appBarLayout()->addWidget(m_menuLable);
    m_bar->appBarLayout()->addStretch(1);
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    m_snkbar->setParent(this);            //消息弹出条

    QWidget *container = new QWidget(this);                   //操作界面容器
    container->setMinimumHeight(500);

    layout->addWidget(m_bar);
    layout->addStretch(1);
    layout->addWidget(container);

    QVBoxLayout *container_layout = new QVBoxLayout;
    container->setLayout(container_layout);
    container->setStyleSheet(QStringLiteral("QWidget { background: white; }"));
    container_layout->setMargin(15);



    m_drawer->setParent(container);
    m_drawer->setClickOutsideToClose(true);
    m_drawer->setOverlayMode(true);
    QVBoxLayout *drawerLayout = new QVBoxLayout;
    drawerLayout->setSpacing(5);
    m_drawer->setDrawerLayout(drawerLayout);
    m_refreshDirBtn = new QPushButton(QStringLiteral("刷新文件夹"));
    m_uploadFileBtn = new QPushButton(QStringLiteral("上传文件至当前目录"));
    m_changeDownloadPathBtn = new QPushButton(QStringLiteral("修改默认下载位置"));
    m_goBackBtn = new QPushButton(QStringLiteral("返回上一级"));
    m_downLoadPathLable = new QLabel(QString("默认下载路径:\n").append(m_defaultDownLoadPath));
    m_downLoadPathLable->setMinimumHeight(30);
    m_downLoadPathLable->setFont(QFont(QStringLiteral("微软雅黑"), 10, QFont::Medium));
    m_downLoadPathLable->setWordWrap(true);
    drawerLayout->addWidget(m_goBackBtn);
    drawerLayout->addStretch(3);
    drawerLayout->addWidget(m_refreshDirBtn);
    drawerLayout->addStretch(3);
    drawerLayout->addWidget(m_uploadFileBtn);
    drawerLayout->addStretch(3);
    drawerLayout->addWidget(m_changeDownloadPathBtn);
    drawerLayout->addStretch(3);
    drawerLayout->addWidget(m_downLoadPathLable);
    m_drawer->setContentsMargins(10, 0, 0, 0);


    container_layout->addWidget(m_fileTable);
    m_fileTable->setColumnCount(7);
    QStringList header;
    header<<""<<"文件名"<<"文件类型"<<"最近修改时间"<<"文件大小"<<""<<"";
    m_fileTable->setHorizontalHeaderLabels(header);
    m_fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fileTable->verticalHeader()->setVisible(false);
    m_fileTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //先自适应宽度
    m_fileTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    m_fileTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fileTable->setShowGrid(false);
    m_fileTable->setStyleSheet(QStringLiteral("QTableWidget {selection-background-color: rgb(185, 205, 241);}"
                             "QScrollBar{ background: white; width:5px}"
                             "QScrollBar::handle:vertical{ background-color: #86dbcb; min-height: 60px ;width:30px }"));
    m_fileTable->setContextMenuPolicy(Qt::CustomContextMenu);                 //右键菜单设置
    m_menu = new QMenu;
    m_menu->setStyleSheet(QStringLiteral("QMenu::item {background : gray;border-color: gray;} "
                        "QMenu {background : gray;}"));
    m_menu->addAction(QStringLiteral("创建新文件夹"), this, &MainWidget::createDirQuest);
    m_menu->addAction(QStringLiteral("重命名"), this, &MainWidget::reNameAction);
    m_menu->addAction(QStringLiteral("刷新"), this, &MainWidget::refreshDirQuest);
    m_menu->addAction(QStringLiteral("删除"), this, &MainWidget::removeQuest);



    connect(m_btn, &QPushButton::pressed, m_drawer, &QtMaterialDrawer::openDrawer);
    connect(m_refreshDirBtn, &QPushButton::pressed, this, &MainWidget::refreshDirQuest);
    connect(m_goBackBtn, &QPushButton::pressed, this, &MainWidget::goBackDir);
    connect(m_fileTable, &MainWidget::customContextMenuRequested, this, &MainWidget::slotContextMenu);
    connect(m_fileTable->itemDelegate(), SIGNAL(closeEditor(QWidget*)), this, SLOT(reNameQuest(QWidget*)));
    connect(m_uploadFileBtn, &QPushButton::pressed, this, &MainWidget::uploadFileQuest);
    connect(m_changeDownloadPathBtn, &QPushButton::pressed, this, &MainWidget::changeDownLoadPath);
    connect(m_fileTable,&QTableWidget::itemDoubleClicked, this, &MainWidget::enterDir);
    connect(m_timer, &QTimer::timeout, this, &MainWidget::uploadFileDataQuest);
    connect(this, &MainWidget::initFriendList, m_shareSelectWidget, &shareSelectWidget::fillFriendList);
    connect(m_shareSelectWidget, &shareSelectWidget::sendSelectedFriend, this, &MainWidget::shareFileQuest);


    refreshDirQuest();

}

MainWidget::~MainWidget()
{
    delete m_goBackBtn;
    delete m_refreshDirBtn;
    delete m_uploadFileBtn;
    delete m_changeDownloadPathBtn;
    delete m_btn;
    delete m_menu;
    delete m_downLoadPathLable;
    delete m_fileTable;
    delete m_circularProgressWidget;
    delete m_shareSelectWidget;
    delete ui;
}

void MainWidget::handleRefreshDir(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    int fileCount = pdu->uiMsgLen / sizeof(FileInfo);
    m_fileTable->setRowCount(fileCount);
    m_fileTable->setColumnCount(7);
    QStringList header;
    header<<""<<"文件名"<<"文件类型"<<"最近修改时间"<<"文件大小"<<""<<"";
    for(int i = 0; i < fileCount; i++ ){
        FileInfo *fileInfoP = (FileInfo*)pdu->caMsg + i;
        addFileRow(i, fileInfoP->fileName, fileInfoP->fileChangeTime,
                   fileInfoP->fileType, fileInfoP->fileSize, fileInfoP->isFile);
    }
}

void MainWidget::handleCreateDir(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, CREATE_OK) == 0){
        sendSnkMessage(QStringLiteral("创建成功"));
        refreshDirQuest();
    }
    else if(strcmp(pdu->caData, CREATE_FAIL) == 0){
        sendSnkMessage(QStringLiteral("创建失败"));
    }
}

void MainWidget::handleRename(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, RENAME_OK) == 0){
        sendSnkMessage(QStringLiteral("文件名已更改"));
        return;
    }
    else if(strcmp(pdu->caData, RENAME_FAIL) == 0){
        sendSnkMessage(QStringLiteral("文件不能重名"));
        refreshDirQuest();
    }
}

void MainWidget::handleRemove(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, REMOVE_OK) == 0){
        sendSnkMessage(QStringLiteral("删除成功"));
        refreshDirQuest();
    }
    else if(strcmp(pdu->caData, REMOVE_FAIL) == 0){
        sendSnkMessage(QStringLiteral("删除失败，请先清空文件夹"));
    }

}

void MainWidget::handleUpLoad(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, UPLOAD_OK) == 0){
        sendSnkMessage(QStringLiteral("上传成功"));
        m_circularProgressWidget->close();
        refreshDirQuest();
    }
    else if(strcmp(pdu->caData, UPLOAD_FAIL) == 0){
        sendSnkMessage(QStringLiteral("上传失败"));
    }

}

void MainWidget::handleDownLoad(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    qint64 fileSize;
    char fileName[128];
    memset(fileName, '\0', 128);
    memcpy(&fileSize, pdu->caData + 128, sizeof(qint64));
    memcpy(fileName, pdu->caData, 128);
    if(fileName == m_DownLoadFileName && fileSize >= 0){//先判断文件名是否一致以及文件大小是否大于等于0
        m_iTotalSize = fileSize;
        TcpClient::getInstance().setDownloadStatus(true);
        QString savePath = QString("%1/%2").arg(m_defaultDownLoadPath).arg(m_DownLoadFileName);
        m_File.setFileName(savePath);
        if(!m_File.open(QIODevice::WriteOnly)){
            sendSnkMessage(QStringLiteral("获取下载路径失败"));
        }
    }
    else{
        sendSnkMessage("文件异常");
    }
}

void MainWidget::handleShareFile(PDU *pdu)
{
    if(pdu == nullptr){
        return;
    }
    if(strcmp(pdu->caData, SHARE_FILE_OK) == 0){
        sendSnkMessage(QStringLiteral("分享成功"));
    }
    else if(strcmp(pdu->caData, SHARE_FILE_FAIL) == 0){
        sendSnkMessage(QStringLiteral("分享失败"));
    }
}

void MainWidget::sendSnkMessage(const QString &msg)
{
    m_snkbar->addMessage(msg);
}



const QString &MainWidget::getDownLoadPath()
{
    return m_defaultDownLoadPath;
}

const QString &MainWidget::getDownLoadFileName()
{
    return m_DownLoadFileName;
}

void MainWidget::setDownLoadFileName(const QString &fileName)
{
    m_DownLoadFileName = fileName;
}



void MainWidget::addFileRow(const int &row, const char fileName[], const char fileChangeTime[],
                            const char fileType[], const size_t &fileSize, const int &isFile)
{
    QTableWidgetItem *icon_item = new QTableWidgetItem;
    QIcon icon_1(isFile ? QStringLiteral(":/icon/resource/fileIcon.png") : QStringLiteral(":/icon/resource/dirIcon.png"));
    QString t_file_type(fileType);
    icon_item->setIcon(icon_1);
    m_fileTable->setItem(row, 0, icon_item);
    m_fileTable->setItem(row, 1, new QTableWidgetItem(QString(fileName)));
    m_fileTable->setItem(row, 2, new QTableWidgetItem(isFile ? t_file_type : QString(QStringLiteral("文件夹"))));
    m_fileTable->setItem(row, 3, new QTableWidgetItem(QString(fileChangeTime)));
    m_fileTable->setItem(row, 4, new QTableWidgetItem(QString("%1字节").arg(fileSize)));
    if(isFile){
        QPushButton *downLoadBtn = new QPushButton;
        QPushButton *shareBtn = new QPushButton;
        QIcon icon_2(QStringLiteral(":/icon/resource/downloadIcon.png"));
        QIcon icon_3(QStringLiteral(":/icon/resource/shareIcon.png"));
        downLoadBtn->setIcon(icon_2);
        shareBtn->setIcon(icon_3);
        downLoadBtn->setStyleSheet(QStringLiteral("QPushButton:hover {background: rgb(171, 171, 171);} QPushButton {border: none;}"));
        shareBtn->setStyleSheet(QStringLiteral("QPushButton:hover {background: rgb(171, 171, 171);} QPushButton {border: none;}"));
        m_fileTable->setCellWidget(row, 5, downLoadBtn);
        m_fileTable->setCellWidget(row, 6, shareBtn);
        connect(downLoadBtn, &QPushButton::pressed, this, &MainWidget::downLoadFileQuest);
        connect(shareBtn, &QPushButton::pressed, this, &MainWidget::shareFileAction);
    }
}

void MainWidget::clearTable()
{
    m_fileTable->clearContents();
    m_fileTable->setRowCount(0);
}


void MainWidget::refreshDirQuest()
{
    size_t pathSize = TcpClient::getInstance().getCurpath().toUtf8().size() + 1;
    PDU* pdu = mk_pdu(pathSize);
    pdu->uiMsgType = ENUM_MSG_TYPE_REFRESH_DIR_REQUEST;
    memcpy(pdu->caMsg, TcpClient::getInstance().getCurpath().toStdString().c_str(), pathSize);      //将要刷新的目录发送
    qDebug()<<"发送刷新目录"<<(char*)pdu->caMsg;
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
    clearTable();           //将原先表格清空
}

void MainWidget::createDirQuest()
{
    size_t pathSize = TcpClient::getInstance().getCurpath().toUtf8().size() + 1;
    PDU* pdu = mk_pdu(pathSize);
    pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
    memcpy(pdu->caMsg, TcpClient::getInstance().getCurpath().toStdString().c_str(), pathSize);      //将要创建文件夹的目录发送
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void MainWidget::reNameQuest(QWidget*)
{
    int curRow = m_fileTable->currentRow();
    QTableWidgetItem *item = m_fileTable->item(curRow, 1);
    QString newName = item->text();
    if(newName.isEmpty()){
        sendSnkMessage(QStringLiteral("文件名不能为空"));
        refreshDirQuest();
        return;
    }else if(newName.toUtf8().size() > 100){
        sendSnkMessage(QStringLiteral("请将文件名控制在30个汉字以内或100个字符以内"));
        refreshDirQuest();
        return;
    }
    size_t pathSize = TcpClient::getInstance().getCurpath().toUtf8().size() + 1;
    PDU* pdu = mk_pdu(pathSize);
    pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_DIR_REQUEST;
    memcpy(pdu->caMsg, TcpClient::getInstance().getCurpath().toStdString().c_str(), pathSize);
    memcpy(pdu->caData, newName.toStdString().c_str(), newName.toUtf8().size() + 1);
    memcpy(pdu->caData + 128, m_preName.toStdString().c_str(), m_preName.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void MainWidget::removeQuest()
{
    int curRow = m_fileTable->currentRow();
    if(curRow < 0){
        sendSnkMessage(QStringLiteral("请选择文件进行删除"));
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("提示");
    msgBox.setInformativeText(QStringLiteral("确定要删除该文件吗？"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Cancel)return;


    QTableWidgetItem *item = m_fileTable->item(curRow, 1);
    QString fileName = item->text();
    size_t pathSize = TcpClient::getInstance().getCurpath().toUtf8().size() + 1;
    PDU* pdu = mk_pdu(pathSize);
    item = m_fileTable->item(curRow, 2);
    if(item->text() == "文件夹"){
        pdu->uiMsgType = ENUM_MSG_TYPE_REMOVE_DIR_REQUEST;
    }
    else{
        pdu->uiMsgType = ENUM_MSG_TYPE_REMOVE_FILE_REQUEST;
    }
    memcpy(pdu->caMsg, TcpClient::getInstance().getCurpath().toStdString().c_str(), pathSize);
    memcpy(pdu->caData, fileName.toStdString().c_str(), fileName.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void MainWidget::uploadFileQuest()
{
    m_uploadFilePath = QFileDialog::getOpenFileName();
    if(!m_uploadFilePath.isEmpty()){
        size_t pathSize = TcpClient::getInstance().getCurpath().toUtf8().size() + 1;
        PDU* pdu = mk_pdu(pathSize);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        int idx = m_uploadFilePath.lastIndexOf("/");
        QString fileName = m_uploadFilePath.right(m_uploadFilePath.size() - idx - 1);
        if(fileName.toUtf8().size() > 100){
            sendSnkMessage(QStringLiteral("请将上传文件名控制在30个汉字以内或100个字符以内"));
        }
        QFile file(m_uploadFilePath);
        qint64 fileSize = file.size();              //获得文件大小
        memcpy(pdu->caMsg, TcpClient::getInstance().getCurpath().toStdString().c_str(), pathSize);
        memcpy(pdu->caData, fileName.toStdString().c_str(), fileName.toUtf8().size() + 1);
        memcpy(pdu->caData + 128, &fileSize, sizeof(qint64));
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = nullptr;

        m_circularProgressWidget->show();
        m_timer->start(1000);
    }
    else{
        sendSnkMessage(QStringLiteral("文件名不能为空"));
    }
}

void MainWidget::uploadFileDataQuest()
{
    m_timer->stop();
    QFile file(m_uploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        sendSnkMessage(QStringLiteral("打开文件失败"));
        return;
    }


    char *pBuffer = new char[4096];     //一次写4096
    qint64 ret = 0;     //实际读到的数据数量
    while(true){

        ret = file.read(pBuffer, 4096);
        if(ret > 0 && ret <= 4096){
            TcpClient::getInstance().getTcpSocket().write(pBuffer, ret);
        }
        else if(ret == 0){
            break;
        }
        else if(ret < 0){
            sendSnkMessage(QStringLiteral("上传文件失败:读取失败"));
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer = nullptr;
}

void MainWidget::downLoadFileQuest()
{
    int curRow = m_fileTable->currentRow();
    QTableWidgetItem *item = m_fileTable->item(curRow, 1);
    QString fileName = item->text();
    m_DownLoadFileName = fileName;
    size_t pathSize = TcpClient::getInstance().getCurpath().toUtf8().size() + 1;
    PDU* pdu = mk_pdu(pathSize);
    pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    memcpy(pdu->caMsg, TcpClient::getInstance().getCurpath().toStdString().c_str(), pathSize);
    memcpy(pdu->caData, fileName.toStdString().c_str(), fileName.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void MainWidget::shareFileQuest(QString friendName)
{
    int curRow = m_fileTable->currentRow();
    QTableWidgetItem *item = m_fileTable->item(curRow, 1);
    QString fileName = item->text();
    size_t pathSize = TcpClient::getInstance().getCurpath().toUtf8().size() + 1;
    PDU* pdu = mk_pdu(pathSize);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    memcpy(pdu->caMsg, TcpClient::getInstance().getCurpath().toStdString().c_str(), pathSize);
    memcpy(pdu->caData, fileName.toStdString().c_str(), fileName.toUtf8().size() + 1);
    memcpy(pdu->caData + 128, friendName.toStdString().c_str(), friendName.toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void MainWidget::shareFileAction()
{  
    emit initFriendList();
    m_shareSelectWidget->show();
}

void MainWidget::reNameAction()
{
    int curRow = m_fileTable->currentRow();
    if(curRow < 0)return;
    QTableWidgetItem *item = m_fileTable->item(curRow, 1);
    m_preName = item->text();
    m_fileTable->setFocus();
    m_fileTable->editItem(m_fileTable->item(curRow, 1));
}

void MainWidget::enterDir(QTableWidgetItem *item)
{
    QTableWidgetItem *t_item = m_fileTable->item(item->row(), 2);
    if(t_item->text() != "文件夹")return;
    t_item = m_fileTable->item(item->row(), 1);
    QString dirName = t_item->text();
    TcpClient::getInstance().changeCurpath(dirName, true);
    m_dirHeight++;
    refreshDirQuest();
}

void MainWidget::goBackDir()
{
    if(!m_dirHeight){
        m_drawer->closeDrawer();
        return;
    }
    TcpClient::getInstance().changeCurpath("", false);
    m_dirHeight--;
    m_drawer->closeDrawer();
    refreshDirQuest();
}

void MainWidget::changeDownLoadPath()
{
    QString strpath = QFileDialog::getExistingDirectory();
    if(strpath.isEmpty()){
        sendSnkMessage(QStringLiteral("目录不能为空"));
    }
    else{
        m_defaultDownLoadPath = strpath;
        QFile file(QStringLiteral("C:/Users/tqzth/source/qt/TcpClient/localDownLoadPath.config"));
        if(file.open(QIODevice::WriteOnly)){
            QTextStream in(&file);
            in<<strpath;
            file.close();
            sendSnkMessage(QStringLiteral("修改默认下载路径成功"));
            m_downLoadPathLable->setText(QString("默认下载路径:\n").append(m_defaultDownLoadPath));
        }
        else{
            sendSnkMessage(QStringLiteral("修改默认下载路径失败"));
        }
    }
}

void MainWidget::slotContextMenu(QPoint pos)
{
    m_menu->exec(QCursor::pos());
}
