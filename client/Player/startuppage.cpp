#include "startuppage.h"

#include <QLabel>
#include <QTimer>

startupPage::startupPage(QDialog *parent)
    : QDialog{parent}
{
    // Qt::FramelessWindowHint：去掉窗⼝的边框，即没有标题栏 Qt::Tool：将窗⼝设置为⼯具窗⼝，图标不在标题栏显⽰
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setFixedSize(1450, 860);
    setStyleSheet("background-color: #FFFFFF");

    //启动界面中间的图片
    QLabel *imglabel = new QLabel(this);
    imglabel->setPixmap(QPixmap(":/images/startupPage/biteshipin.png"));
    imglabel->move(524,374);
}

void startupPage::startup()
{
    QTimer *timer = new QTimer();
    connect(timer, &QTimer::timeout, this, [=]{
        timer->stop();
        delete timer;
        close();
    });

    // 启动定时器，超时时⻓为2秒
    timer->start(2000);

}
