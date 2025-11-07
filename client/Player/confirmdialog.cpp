#include "confirmdialog.h"

#include <QPushButton>
#include "player.h"

ConfirmDialog::ConfirmDialog(QWidget *parent)
    : QDialog{parent}
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(1460, 865);

    //Player* Player = Player::getInstance();
    //QPoint pos = Player->mapToGlobal(QPoint(0, 0));
    //move(pos);

    this->setParent(Player::getInstance());
    this->move(0, 0);

    // 遮罩层
    QWidget* bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->setFixedSize(maximumSize());
    bg->setStyleSheet("#bg{background: rgba(0,0,0,0.3);}");

    // ⽩⾊窗⼝区域
    QFrame* frame = new QFrame(bg);
    frame->setObjectName("container");
    frame->setFixedSize(358, 169);
    frame->move(546, 346);
    frame->setStyleSheet("#container{background-color:#FFFFFF;"
                         "border-radius:10px;}"
                         "*{font-size:16px;" "font-family:微软雅⿊;}");

    // 窗⼝⽂本
    textLabel = new QLabel("确定审核通过吗?", frame);
    textLabel->setObjectName("operatorText");
    textLabel->setStyleSheet("color:#000000");
    textLabel->move(115, 40);

    // 确定和取消按钮
    QPushButton* confirmBtn = new QPushButton("确认", frame);
    QPushButton* cancelBtn = new QPushButton("取消", frame);

    confirmBtn->setStyleSheet("background-color:#3ECEFF;"
                              "border-radius:8px;"
                              "color:#FFFFFF;");
    confirmBtn->setFixedSize(100, 38);
    confirmBtn->move(194, 111);

    connect(confirmBtn, &QPushButton::clicked, this, [=]{
        isConfirm = true;
        cancelBtn->setStyleSheet("border-radius:8px;"
                                "border: 1px solid #DEDEDE;");
        confirmBtn->setStyleSheet("background-color:#3ECEFF;"
                                  "border-radius:8px;" "color:#FFFFFF;");
        close();
    });

    cancelBtn->setStyleSheet("border-radius:8px;"
                             "border: 1px solid #DEDEDE;");
    cancelBtn->setFixedSize(100, 38);
    cancelBtn->move(64, 111);

    connect(cancelBtn, &QPushButton::clicked, this, [=]{
        isConfirm = false;
        confirmBtn->setStyleSheet("border-radius:8px;"
                                  "border: 1px solid #DEDEDE;");
        cancelBtn->setStyleSheet("background-color:#3ECEFF;"
                                 "border-radius:8px;"
                                 "color:#FFFFFF;");
        close();
    });
}

bool ConfirmDialog::getConfirm() const
{
    return isConfirm;
}

void ConfirmDialog::setText(const QString &text)
{
    textLabel->setText(text);
}
