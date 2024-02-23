#ifndef OPERATEWIDGET_H
#define OPERATEWIDGET_H

#include <QWidget>
#include "friendWidget.h"
#include "mainWidget.h"
#include "downloadpathwidget.h"
#include <QQueue>

namespace Ui {
class OperateWidget;
}

class OperateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OperateWidget(QWidget *parent = nullptr);
    ~OperateWidget();
    static OperateWidget& getInstance();
    FriendWidget* getFriendWidget();
    MainWidget* getMainWidget();



private slots:
    void on_change_main_btn_clicked();

    void on_change_friend_btn_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::OperateWidget *ui;
    FriendWidget *m_FriendWidget;
    MainWidget *m_MainWidget;
};

#endif // OPERATEWIDGET_H
