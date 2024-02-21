#include "downloadpathwidget.h"
#include "ui_downloadpathwidget.h"
#include <QDebug>
#include <QFileDialog>
#include <QFile>

DownLoadPathWidget::DownLoadPathWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownLoadPathWidget)
{
    ui->setupUi(this);
    QFile file(":/config/localDownLoadPath.config");
    QString strData;
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        strData = baData.toStdString().c_str();
        file.close();
    }
    else{
        qDebug()<<"file open error";
    }
    ui->pathLine->setText(strData);
}

DownLoadPathWidget::~DownLoadPathWidget()
{
    delete ui;
}

void DownLoadPathWidget::on_changePathBtn_clicked()
{
    QFile file("C:/Users/tqzth/source/qt/TcpClient/localDownLoadPath.config");
    if(file.open(QIODevice::WriteOnly)){
        QString path = QFileDialog::getSaveFileName();
        QTextStream in(&file);
        in<<path<<endl;
        file.close();
    }
    else{
        qDebug()<<"file open error";
    }
}
