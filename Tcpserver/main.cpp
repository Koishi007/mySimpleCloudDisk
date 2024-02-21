#include "tcpserver.h"

#include <QApplication>

#include "connectDB.h"

#include <QSqlQuery>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServer w;
    connectDB::getInstance().init();
    w.show();
    return a.exec();
}
