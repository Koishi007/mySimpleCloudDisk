#include "tcpclient.h"

#include <QApplication>

#include <operateWidget.h>

#include "framelessWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient::getInstance().show();
    return a.exec();
}
