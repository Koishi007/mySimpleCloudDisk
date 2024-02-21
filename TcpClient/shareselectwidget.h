#ifndef SHARESELECTWIDGET_H
#define SHARESELECTWIDGET_H


#include <QWidget>
#include <QString>

namespace Ui {
class shareSelectWidget;
}

class shareSelectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit shareSelectWidget(QWidget *parent = nullptr);
    ~shareSelectWidget();

signals:
    void sendSelectedFriend(QString friendName);

public slots:
    void fillFriendList();

private slots:
    void on_yes_btn_clicked();



private:
    Ui::shareSelectWidget *ui;
};

#endif // SHARESELECTWIDGET_H
