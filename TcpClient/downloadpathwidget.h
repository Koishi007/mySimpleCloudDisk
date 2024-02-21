#ifndef DOWNLOADPATHWIDGET_H
#define DOWNLOADPATHWIDGET_H

#include <QWidget>

namespace Ui {
class DownLoadPathWidget;
}

class DownLoadPathWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownLoadPathWidget(QWidget *parent = nullptr);
    ~DownLoadPathWidget();

private slots:
    void on_changePathBtn_clicked();

private:
    Ui::DownLoadPathWidget *ui;
};

#endif // DOWNLOADPATHWIDGET_H
