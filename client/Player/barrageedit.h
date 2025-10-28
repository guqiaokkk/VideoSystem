#ifndef BARRAGEEDIT_H
#define BARRAGEEDIT_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class BarrageEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit BarrageEdit(QWidget *parent = nullptr);

    // 发送弹幕按钮点击
    void onSendBulletScreenBtn();

signals:
    // 通知PlayerPage发送该条弹幕
    void sendBulletScreen(const QString &text);

private:
    QPushButton *sendBSBtn;
};

#endif // BARRAGEEDIT_H
