#include "barrageedit.h"
#include "toast.h"
#include "./model/datacenter.h"

#include <QHBoxLayout>

BarrageEdit::BarrageEdit(QWidget *parent)
    : QLineEdit{parent}
{
    // 初始化编辑框
    this->setFixedSize(1030, 32);
    this->setPlaceholderText("pilipili-(゜-゜)つロ乾杯~");
    this->setMaxLength(30);
    this->setTextMargins(12, 6, 0, 7);

    // 初始化发送按钮
    sendBSBtn = new QPushButton(this);
    sendBSBtn->setFixedSize(58, 28);
    sendBSBtn->setText("发送");
    sendBSBtn->setCursor(QCursor(Qt::ArrowCursor));
    sendBSBtn->setStyleSheet(
        "background-color : #3ECEFE;"
        "border-radius : 4px;"
        "color : #FFFFFF;"
        "font-size : 14px;"
        "font-family : 微软雅⿊;"
        );
    sendBSBtn->move(width()-sendBSBtn->width() - 2, 2);

    // 实例化登录窗⼝实例
    login = new Login();
    connect(login, &Login::loginSuccess, this, [=](){
        setPlaceholderText("pilipili-(゜-゜)つロ乾杯~");
        sendBSBtn->setText("发送");
    });

    // 发送弹幕按钮点击
    connect(sendBSBtn, &QPushButton::clicked, this, &BarrageEdit::onSendBulletScreenBtn);
}

BarrageEdit::~BarrageEdit()
{
    delete login;
}

void BarrageEdit::onSendBulletScreenBtn()
{
    // 未登录时，让⽤⼾先登录
    if(sendBSBtn->text() == "登录")
    {
        Toast::showMessage("请先登录或注册", login);
    }
    else
    {
        QString barrageText = this->text();
        if(barrageText.isEmpty())
        {
            setToolTip("请输⼊要发送的弹幕~~~");
            return;
        }
    }
    emit sendBulletScreen(this->text());
    this->setText("");
}

// 编辑框⽀持⿏标点击
void BarrageEdit::mousePressEvent(QMouseEvent *event)
{
    // 当点击编辑框时，需要先检测⽤⼾是否登录
    // 未登录时，输⼊框提⽰"请先登录或注册
    auto dataCenter = model::DataCenter::getInstance();
    auto mySelf = dataCenter->getMyselfInfo();
    if(mySelf->isTempUser())
    {
        if(placeholderText() != "请先登录或注册")
        {
            setPlaceholderText("请先登录或注册");
            sendBSBtn->setText("登录");
        }
        else
        {
            Toast::showMessage("请先登录或注册", login);
        }
    }

}
