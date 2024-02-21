#ifndef FILESHAREINFOWIDGET_H
#define FILESHAREINFOWIDGET_H

#include <QWidget>

namespace Ui {
class FileShareInfoWidget;
}

class FileShareInfoWidget : public QWidget
{
    Q_OBJECT

public slots:
    void initFileShareInfo(QString fileName, QString sender, qint64 fileSize);

public:
    explicit FileShareInfoWidget(QWidget *parent = nullptr);
    ~FileShareInfoWidget();

private slots:
    void on_refuse_btn_clicked();

    void on_accept_btn_clicked();

private:
    Ui::FileShareInfoWidget *ui;
};

#endif // FILESHAREINFOWIDGET_H
