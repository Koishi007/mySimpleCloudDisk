#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "qtmaterialdrawer.h"
#include "qtmaterialappbar.h"
#include <QLabel>
#include "qtmaterialiconbutton.h"
#include <lib/qtmaterialtheme.h>
#include "qtmaterialsnackbar.h"
#include "qtmaterialcircularprogress.h"
#include <QColorDialog>
#include "protocol.h"
#include <QTableWidget>
#include <QPushButton>
#include <QMenu>
#include <QTimer>
#include <QFile>
#include "framelessWidget.h"
#include "shareSelectWidget.h"



namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void handleRefreshDir(PDU* pdu);        //处理服务器的刷新目录回应
    void handleCreateDir(PDU* pdu);         //处理服务器的创建文件夹回应
    void handleRename(PDU* pdu);            //处理服务器的重命名回应
    void handleRemove(PDU* pdu);            //处理服务器的删除回应
    void handleUpLoad(PDU* pdu);            //处理服务器的上传回应
    void handleDownLoad(PDU* pdu);          //处理服务器的下载回应
    void handleShareFile(PDU* pdu);         //处理服务器的分享回应
    void sendSnkMessage(const QString& msg);    //弹出提示条
    const QString& getDownLoadPath();
    const QString& getDownLoadFileName();
    void setDownLoadStatus(bool status);
    void setDownLoadFileName(const QString& fileName);      //这个接口开放给分享界面用于下载分享文件

    QFile m_File;                                           //操作下载文件的Qfile
    qint64 m_iTotalSize = 0;                                   //下载总文件大小
    qint64 m_iRevcdSize = 0;                                    //已收到大小


private:
    void addFileRow(const int &row, const char filename[], const char fileChangeTime[],
                    const char fileType[], const size_t &fileSize, const int &isFiile);
    void clearTable();

signals:
    void initFriendList();

public slots:
    void refreshDirQuest();                             //发送刷新目录请求
    void createDirQuest();                              //发送创建新文件夹请求
    void reNameQuest(QWidget*);                         //发送重命名请求
    void removeQuest();                              //删除请求
    void uploadFileQuest();                             //上传文件请求
    void uploadFileDataQuest();                         //上传文件数据部分请求
    void downLoadFileQuest();                           //下载文件请求
    void shareFileQuest(QString friendName);                              //分享文件请求

    void reNameAction();                                //重命名动作
    void shareFileAction();                             //分享文件动作
    void enterDir(QTableWidgetItem *item);              //进入文件夹
    void goBackDir();                                   //返回上一级
    void changeDownLoadPath();


    void slotContextMenu(QPoint pos);                   //显示右键菜单

private:
    QString m_preName;                                    //重命名文件时用到的临时变量
    int m_dirHeight = 0;                                  //用户目录树高度
    QString m_defaultDownLoadPath;
    QTimer *m_timer;
    QString m_uploadFilePath;
    QString m_DownLoadFileName;


    Ui::MainWidget *ui;
    QtMaterialDrawer *m_drawer;
    QtMaterialAppBar *m_bar;
    QtMaterialSnackbar *m_snkbar;
    QTableWidget *m_fileTable;
    QMenu *m_menu;
    QPushButton *m_refreshDirBtn;
    QPushButton *m_uploadFileBtn;
    QPushButton *m_changeDownloadPathBtn;
    QPushButton *m_goBackBtn;
    QPushButton *m_ejectBtn;                                   //弹出栏按钮
    QLabel *m_downLoadPathLable;
    framelessWidget *m_circularProgressWidget;
    shareSelectWidget *m_shareSelectWidget;
    QLabel *m_menuLable;

};

#endif // MAINWIDGET_H
