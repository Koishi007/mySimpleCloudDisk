#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QString>
#include <QTcpSocket>
#include <QStackedWidget>
#include "operatewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();

    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();
    TcpClient *getTcpClientP();
    const QString &getCurpath();
    const QString &getUserName();

    void changeCurpath(const QString &dirName, bool flag);
    void setDownloadStatus(bool status);

public slots:
    void showConnect();

    void rec_Msg();

signals:


private slots:


    void on_login_in_btn_clicked();

    void on_reg_btn_clicked();

    void on_login_out_btn_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;    //ip
    quint16 m_usPort;   //端口号
    QTcpSocket m_tcpSocket; //套接字
    bool m_isdownLoad;
    QString m_curPath;    //用户实时目录
    QString m_name;       //用户名
};
#endif // TCPCLIENT_H
