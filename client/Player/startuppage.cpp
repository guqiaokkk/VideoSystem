#include "startuppage.h"

#include "./model/datacenter.h"

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

    // 临时⽤⼾登录成功信号槽绑定
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::loginTempUserDone, this, [=](){
        loginSuccess = true;
    });
}

void startupPage::startup()
{
    QTimer *timer = new QTimer();

    // 在时间达到后，重复每隔2s触发timeout信号
    timer->setSingleShot(false); // 设置为周期定时器
    connect(timer, &QTimer::timeout, this, [=]{
        // 临时⽤⼾登录成功，删除定时器，关闭该⻚⾯
        if(loginSuccess){
        timer->stop();
        delete timer;
        close();
        }
    });

    // 启动定时器，超时时⻓为2秒
    timer->start(2000);

    // 发送临时⽤⼾登录请求
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loginTempUserAsync();
}
