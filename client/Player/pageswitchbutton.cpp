#include "pageswitchbutton.h"
#include "util.h"

PageSwitchButton::PageSwitchButton(QWidget *parent)
    : QPushButton{parent}
{
    // 设置按钮的⼤⼩
    setFixedSize(48,46);

    //图片
    btnImg = new QLabel(this);
    btnImg->setGeometry((48-24)/2, 0, 24, 24);
    //文本
    btnTittle = new QLabel(this);
    btnTittle->setGeometry(0, 30, 48, 16);
    btnTittle->setAlignment(Qt::AlignCenter);

    //去边框
    setStyleSheet("border:none;");
}

void PageSwitchButton::setImageAndText(const QString &ImgPath, const QString &text, int pageId)
{
    btnTittle->setText(text);
    setImage(ImgPath);

    //将按钮与Player中的⻚⾯关联
    this->pageId = pageId;
}

void PageSwitchButton::mousePressEvent(QMouseEvent *event)
{
    (void)event;
    setTextColor("#000000");
    //发射切换⻚⾯信号
    emit switchPage(pageId);

    LOG() << pageId << "clicked.";
}

void PageSwitchButton::setImage(const QString &ImgPath)
{
    btnImg->setPixmap(QPixmap(ImgPath));
}

void PageSwitchButton::setTextColor(const QString &textColor)
{
   btnTittle->setStyleSheet("font-family: 微软雅⿊;"
                             "font-size: 12px;"
                             "font-style: normal;"
                             "font-weight:bold;" "color: " + textColor + ";");
}

int PageSwitchButton::getPageId() const
{
    return pageId;
}
