#include "login.h"
#include "ui_login.h"
#include "model/datacenter.h"
#include "util.h"
#include "toast.h"

#include <QGraphicsDropShadowEffect>
#include <QRegularExpressionValidator>

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    // 去除窗⼝边框，并设置窗⼝为模态对话框
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_ShowModal, true);

    // 窗⼝加上阴影效果
    setAttribute(Qt::WA_TranslucentBackground); // 阴影效果必须要窗⼝透明
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);
    shadowEffect->setBlurRadius(5); shadowEffect->setOffset(0);
    ui->loginBg->setGraphicsEffect(shadowEffect);

    // 密码登录 和 短信登录，假设默认为短信登录
    ui->messageWidget->show();
    ui->passwordWidget->hide();

    // lineEdit限制
    ui->passwordNum->setEchoMode(QLineEdit::Password);

    // ⼿机编辑框和账号限制
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->accountNum->setValidator(validator); ui->emailNum->setValidator(validator);

    // 创建整数验证器，输⼊为0~999999
    QIntValidator* intValid = new QIntValidator(0, 999999, ui->authcode);
    ui->authcode->setValidator(intValid);
    ui->authcode->setMaxLength(6);


    // 最⼩化和关闭按钮槽函数
    connect(ui->min, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quit, &QPushButton::clicked, this, &QWidget::close);

    // 切换账号密码登录
    connect(ui->passwordBtn, &QPushButton::clicked, this, &Login::onPasswordBtnClicked);

    // 切换短信登录
    connect(ui->messageBtn, &QPushButton::clicked, this, &Login::onMessageBtnClicked);



    // 获取短信验证码按钮点击
    connect(ui->authcodeBtn, &QPushButton::clicked, this, &Login::onAuthcodeBtnClicked);

    // 获取短信验证码成功
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getAuthcodeDone, this, [=](const QString &authcodeId){
        this->authcodeId = authcodeId;
        LOG()<<"获取短信验证码成功!!!";
    });

    // 注册/登录按钮点击
    connect(ui->loginOrRegister, &QPushButton::clicked, this, &Login::onLoginAndRegisterBtnClicked);

    // 短信登录成功
    connect(dataCenter, &model::DataCenter::loginWithMessageDone, this, [=](){
        emit loginSuccess();    //由myselfWidget来处理，重新获取登陆成功的用户的个人信息以及视频列表
        this->close();
    });
    // 短信登录失败
    connect(dataCenter, &model::DataCenter::loginWithMessageFailed, this, [=](const QString &errorInfo){
       Toast::showMessage(errorInfo);
    });


    // ⽴即注册按钮点击
    connect(ui->registerNow, &QPushButton::clicked, this, &Login::onRegisterNowBtnClicked);

    // 密码登录
    connect(ui->loginNow, &QPushButton::clicked, this, &Login::onLoginNowBtnClicked);
    // 密码登录成功
    connect(dataCenter, &model::DataCenter::loginWithPasswordDone, this, [=](){
        emit loginSuccess();    //由myselfWidget来处理，重新获取登陆成功的用户的个人信息以及视频列表
        this->close();
    });
    // 密码登录失败
    connect(dataCenter, &model::DataCenter::loginWithPasswordFailed, this, [=](const QString &errorInfo){
        Toast::showMessage(errorInfo);
    });
}

Login::~Login()
{
    delete ui;
}

void Login::onPasswordBtnClicked()
{
    ui->messageWidget->hide();
    ui->passwordWidget->show();
    ui->passwordBtn->setStyleSheet("QPushButton{" "color : #3ECEFE;" "font-weight:bold;" "border : none;" "border-bottom : 6px solid #3ECEFE;}");
    ui->messageBtn->setStyleSheet("QPushButton{" "color : #222222;" "border : none;" "border-bottom: 2px solid #B5ECFF;}");
}

void Login::onMessageBtnClicked()
{
    ui->messageWidget->show();
    ui->passwordWidget->hide();
    ui->messageBtn->setStyleSheet("QPushButton{" "color : #3ECEFE;" "font-weight:bold;" "border : none;" "border-bottom : 6px solid #3ECEFE;}");
    ui->passwordBtn->setStyleSheet("QPushButton{" "color : #222222;" "border : none;" "border-bottom: 2px solid #B5ECFF;}");
}

void Login::onAuthcodeBtnClicked()
{
    const QString &email = ui->emailNum->text();
    if(email.isEmpty())
    {
        Toast::showMessage("⼿机号不能为空");
    }
    else
    {
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->getAuthcodeAsync(ui->emailNum->text());
    }
}

// 短信登录：登录和注册
void Login::onLoginAndRegisterBtnClicked()
{
    // 短信登录
    // 1. 如果⽤⼾未注册，则服务端先注册后登录
    // 2. 如果⽤⼾已注册，则直接登录
    const QString &email = ui->emailNum->text();
    const QString &authcode = ui->authcode->text();

    if(email.isEmpty() || authcode.isEmpty())
    {
        LOG()<<"⼿机号或验证码不能为空";
        return;
    }

    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loginWithMessageAsync(email, authcode, authcodeId);
}

void Login::onRegisterNowBtnClicked()
{
    ui->registerNow->setStyleSheet("#registerNow{"
                                   "color : #FFFFFF;"
                                   "background-color : #3ECEFE;"
                                   "border-radius : 36px;}");
    ui->loginNow->setStyleSheet("#loginNow{"
                                "color : #222222;"
                                "background-color : #FFFFFF;"
                                "border-radius : 36px;"
                                "border: 1px solid #C2C9D9;}");

    // 从当前页面切换到短信登录⻚⾯
    onMessageBtnClicked();
}

void Login::onLoginNowBtnClicked()
{
    ui->registerNow->setStyleSheet("#registerNow{"
                                   "color : #222222;"
                                   "background-color : #FFFFFF;"
                                   "border-radius : 36px;"
                                   "border: 1px solid #C2C9D9;}");
    ui->loginNow->setStyleSheet("#loginNow{"
                                "color : #FFFFFF;"
                                "background-color : #3ECEFE;"
                                "border-radius : 36px;}");

    // 账号密码登录
    QString accountNum = ui->accountNum->text();
    QString password = ui->passwordNum->text();

    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loginWithPasswordAsync(accountNum, password);
}
