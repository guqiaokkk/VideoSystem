#include "bulletscreenitem.h"
#include <QHBoxLayout>

BulletScreenItem::BulletScreenItem(QWidget *parent)
    : QFrame{parent}
{
    setObjectName("bulletScreenWnd");
    this->setFixedHeight(38);
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    setLayout(hLayout);

    // 弹幕中⽤⼾头像
    imgLabel = new QLabel(this);
    imgLabel->setFixedSize(30, 30);
    imgLabel->setScaledContents(true);
    imgLabel->hide();

    // 弹幕中⽂本
    text = new QLabel(this);
    text->setStyleSheet("font-family : 微软雅⿊;"
                        "font-size : 18px;"
                        "color : #FFFFFF;"
                        "border : none;");
    text->adjustSize();                             //  自动调整控件大小以适应其内容

    // 将元素添加到⽔平布局器中
    hLayout->addWidget(imgLabel);
    hLayout->addWidget(text);
    hLayout->setContentsMargins(10, 4, 19, 4);
    hLayout->setSpacing(8);
}

void BulletScreenItem::setBulletScreenText(const QString &content)
{
    text->setText(content);
    text->adjustSize();
}

void BulletScreenItem::setBulletScreenIcon(QPixmap &pixmap)
{
    imgLabel->setPixmap(pixmap);
    imgLabel->show();

    // 因为如果有图片显示了，说明这条弹幕是我自己发的，所以要给整个弹幕加上边框
    this->setStyleSheet("#bulletScreenWnd{" "border : 1px solid #3ECEFE;" "border-radius : 19px;}");
}

void BulletScreenItem::setBulletScreenAnimation(int x, int duration)
{
    animation = new QPropertyAnimation(this, "pos", this);
    animation->setDuration(duration);
    animation->setStartValue(QPoint(x, 0));
    animation->setEndValue(QPoint(0 - this->width(), 0));

    connect(animation, &QPropertyAnimation::finished, this, [=](){
        delete this;
    });
}

void BulletScreenItem::startAnimation()
{
    show();
    animation->start();
}
