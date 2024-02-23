#include "fileShareInfoWidget.h"
#include "ui_fileshareinfowidget.h"
#include "tcpclient.h"
#include "protocol.h"
#include "operateWidget.h"

void FileShareInfoWidget::initFileShareInfo(QString fileName, QString sender, qint64 fileSize)
{
    ui->file_name->setText(fileName);
    ui->sender->setText(sender);
    ui->file_size->setText(QString::number(fileSize) + QStringLiteral("字节"));
}

FileShareInfoWidget::FileShareInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileShareInfoWidget)
{
    ui->setupUi(this);
    ui->file_name->setWordWrap(true);
    ui->file_size->setWordWrap(true);
}

FileShareInfoWidget::~FileShareInfoWidget()
{
    delete ui;
}

void FileShareInfoWidget::on_refuse_btn_clicked()
{
    PDU* pdu = mk_pdu(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_REFUSE_REQUEST;
    memcpy(pdu->caData, ui->file_name->text().toStdString().c_str(), ui->file_name->text().toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void FileShareInfoWidget::on_accept_btn_clicked()
{
    QString t_path = TcpClient::getInstance().getCurpath().left(TcpClient::getInstance().getCurpath().indexOf(QStringLiteral("userData")) + 8);
    QString path = QString(QStringLiteral("%1/%2")).arg(t_path).arg(ui->sender->text());
    size_t pathSize = path.toUtf8().size() + 1;
    OperateWidget::getInstance().getMainWidget()->setDownLoadFileName(ui->file_name->text());
    PDU* pdu = mk_pdu(pathSize);
    pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    memcpy(pdu->caMsg, path.toStdString().c_str(), pathSize);
    memcpy(pdu->caData, ui->file_name->text().toStdString().c_str(), ui->file_name->text().toUtf8().size() + 1);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}
