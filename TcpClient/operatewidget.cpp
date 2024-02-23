#include "operateWidget.h"
#include "ui_operatewidget.h"
#include "protocol.h"
#include <QDebug>
#include <QFileDialog>
#include <QFile>

OperateWidget::OperateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OperateWidget)
{
    ui->setupUi(this);
    this->setWindowTitle("操作界面");
    m_FriendWidget = new FriendWidget;
    m_MainWidget = new MainWidget;
    ui->conent_widget->resize(1000,600);
    ui->conent_widget->addWidget(m_FriendWidget);
    ui->conent_widget->addWidget(m_MainWidget);
    ui->conent_widget->setCurrentWidget(m_MainWidget);
    setFixedSize(1120,650);
    this->setStyleSheet("background-color: white;");

}

OperateWidget::~OperateWidget()
{
    delete ui;
    delete m_FriendWidget;
    delete m_MainWidget;
    qDebug()<<"OpeW析构";
}

OperateWidget &OperateWidget::getInstance()
{
    static OperateWidget instance;
    return instance;
}

FriendWidget *OperateWidget::getFriendWidget()
{
    return m_FriendWidget;
}

MainWidget *OperateWidget::getMainWidget()
{
    return m_MainWidget;
}


void OperateWidget::on_change_main_btn_clicked()
{
    ui->conent_widget->setCurrentWidget(m_MainWidget);
    m_FriendWidget->setFocus();
}

void OperateWidget::on_change_friend_btn_clicked()
{
    ui->conent_widget->setCurrentWidget(m_FriendWidget);
    m_FriendWidget->setFocus();
}

void OperateWidget::on_pushButton_3_clicked()
{

}
