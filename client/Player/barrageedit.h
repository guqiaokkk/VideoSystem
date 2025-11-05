#ifndef BARRAGEEDIT_H
#define BARRAGEEDIT_H

#include "login.h"
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class BarrageEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit BarrageEdit(QWidget *parent = nullptr);
    ~BarrageEdit();

    // 发送弹幕按钮点击
    void onSendBulletScreenBtn();

    // 处理编辑框点击
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    // 通知PlayerPage发送该条弹幕
    void sendBulletScreen(const QString &text);

private:
    QPushButton *sendBSBtn;

    // 登录窗⼝
    Login *login;

};

#endif // BARRAGEEDIT_H
