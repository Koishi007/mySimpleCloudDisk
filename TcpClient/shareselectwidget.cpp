#include "shareSelectWidget.h"
#include "ui_shareselectwidget.h"
#include "operateWidget.h"
#include <QSet>
#include <QIcon>
#include <QMessageBox>

shareSelectWidget::shareSelectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::shareSelectWidget)
{
    ui->setupUi(this);
}

shareSelectWidget::~shareSelectWidget()
{
    delete ui;
}

void shareSelectWidget::fillFriendList()
{
    ui->friend_list->clear();
    QSet<QString> friendSet = OperateWidget::getInstance().getFriendWidget()->getFriendSet();
    foreach(auto it, friendSet){
        QIcon icon(QStringLiteral(":/icon/resource/friendIcon.png"));
        QListWidgetItem *item = new QListWidgetItem(icon, it);
        ui->friend_list->addItem(item);
    }
}

void shareSelectWidget::on_yes_btn_clicked()
{
    int curRow = ui->friend_list->currentRow();
    if(curRow < 0){
        QMessageBox::warning(this, "提示", "需要选择一位好友哦");
        return;
    }
    QListWidgetItem *item = ui->friend_list->item(curRow);
    emit sendSelectedFriend(item->text());
    this->hide();
}

