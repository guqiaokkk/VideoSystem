#include "barrageedit.h"

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
}
