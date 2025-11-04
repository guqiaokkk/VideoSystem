#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    // 选择密码登录
    void onPasswordBtnClicked();

    // 选择短信登录
    void onMessageBtnClicked();

    // 获取短信验证码
    void onAuthcodeBtnClicked();

    // 登录注册
    void onLoginAndRegisterBtnClicked();

    // ⽴即注册
    void onRegisterNowBtnClicked();

    // 密码登录
    void onLoginNowBtnClicked();

signals:
    // 登录成功信号
    void loginSuccess();

private:
    Ui::Login *ui;

    // 获取短信验证码成功后返回
    QString authcodeId;


};

#endif // LOGIN_H
