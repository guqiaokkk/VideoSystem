#include "pagebutton.h"



PageButton::PageButton(int page, QWidget *parent)
    : QPushButton{parent}
{
    this->page = page;

    // 设置按钮的图标尺⼨ 和 按钮⼤⼩
    this->setIconSize(QSize(12, 12));
    this->setFixedSize(QSize(32, 32));

    // 设置按钮上显⽰⽂本
    if(this->isFolded()){
        this->setText("...");
    }
    else{
        this->setText(QString::number(page));
    }

    // 设置按钮是否为选中状态
    setActive(isActiveBtn);
}

void PageButton::setActive(bool active)
{
    this->isActiveBtn = active;
    if (active) {
        this->setStyleSheet("QPushButton {" "color: #FFFFFF; "
                            "background-color: #3ECEFE; "
                            "border: 1px solid #3ECEFE; "
                            "" "border-radius: 2px;}");
    }
    else { this->setStyleSheet("QPushButton {" "color: #000000; "
                            "background-color: #FFFFFF; "
                            "border: 1px solid #D9D9D9; "
                            "border-radius: 2px;}");
    }
}

void PageButton::setFolded(bool folded)
{
    this->isFoldedBtn = folded;
    if (folded) {
        this->setText("...");
    }
    else {
        this->setText(QString::number(page));
    }
}

void PageButton::setPage(int page)
{
    this->page = page;
    this->setText(QString::number(page));
}

int PageButton::getPage() const
{
    return page;
}

bool PageButton::isActived() const
{
    return isActiveBtn;
}

bool PageButton::isFolded() const
{
    return isFoldedBtn;
}
