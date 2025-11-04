#include "startuppage.h"

#include "./model/datacenter.h"
#include "util.h"

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

        // 临时⽤⼾登录成功，则当前⽤⼾⾝份为临时⽤⼾,临时⽤⼾在服务器上没有个⼈信息，因此从服务器上⽆法获取到临时⽤⼾个⼈信息
        // 此处为当前的临时⽤⼾设置⼀个userInfo结构，后续⾸⻚、我的⻚⾯中需要区分出临时⽤⼾
        dataCenter->buildTempUserInfo();

        // 构建临时⽤⼾信息成功
        mySelfInfoSuccess = true;
    });

    // 会话登录成功
    connect(dataCenter, &model::DataCenter::loginWithSessionDone, this, [=](bool isTempUser){
        loginSuccess = true;
        // 临时
        if(isTempUser)
        {
            dataCenter->buildTempUserInfo();
            mySelfInfoSuccess = true;
        }
        else
        {
            dataCenter->getMyselfInfoAsync();
        }
    });
    // 会话登录失败
    connect(dataCenter, &model::DataCenter::loginWithSessionFailed, this, [=](const QString &reason){
        LOG()<<"session login failed";
        // 如果会话登录失败，则使⽤临时⽤⼾⾝份登录
        dataCenter->loginTempUserAsync();
    });

    // 获取个⼈信息成功，设置myselfInfoSuccess为true
    connect(dataCenter, &model::DataCenter::getMyselfInfoDone, this, [=](){
        mySelfInfoSuccess = true;
    });

}

void startupPage::startup()
{
    QTimer *timer = new QTimer();

    // 在时间达到后，重复每隔2s触发timeout信号
    timer->setSingleShot(false); // 设置为周期定时器
    connect(timer, &QTimer::timeout, this, [=]{
        // // 登录成功 并且 获取到 ⽤⼾信息 再关闭定时器，删除定时器，关闭该⻚⾯
        if(loginSuccess && mySelfInfoSuccess){
        timer->stop();
        delete timer;
        close();
        }
    });

    // 启动定时器，超时时⻓为2秒
    timer->start(2000);

    // 如果sessionId为空，说明用户是第一次使用Player，此时以临时用户方式登录
    // 如果sessionId非空，但是当前用户是临时用户，此时以临时用户方式登录
    // 如果sessionId非空，当前用户也是非临时用户，此时使用session登录
    auto dataCenter = model::DataCenter::getInstance();
    QString sessionId = dataCenter->getLogingSessionId();
    if(sessionId.isEmpty() || dataCenter->getMyselfInfo()->isTempUser())
    {
        // 临时登录，从服务器获取sessionId
        dataCenter->loginTempUserAsync();
    }
    else
    {
        // 直接会话登录
        dataCenter->loginSessionAsync();
    }
}
