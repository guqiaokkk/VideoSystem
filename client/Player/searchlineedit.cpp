#include "searchlineedit.h"

#include "util.h"

#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit{parent}
{
    // 搜索框图标
    QLabel *searchImg = new QLabel(this);
    searchImg->setFixedSize(16, 16);
    searchImg->setPixmap(QPixmap(":/images/homePage/sousuo.png"));

    // 搜索按钮
    QPushButton *searchBtn = new QPushButton(this);
    searchBtn->setText("搜索");
    searchBtn->setFixedSize(62,32);
    searchBtn->setCursor(QCursor(Qt::ArrowCursor));
    searchBtn->setStyleSheet("background-color : #3ECEFE;"
                             "border-radius : 16px;"
                             "font-family : 微软雅⿊;"
                             "font-size : 14px;"
                             "color : #FFFFFF;"
                             "font-style : normal;");

    //搜索列
    this->setPlaceholderText("请搜索你感兴趣的视频");
    this->setFixedWidth(320);
    this->setFixedHeight(36);
    setObjectName("search");
    this->setStyleSheet("#search{background-color : #FFFFFF;"
                        "border-radius : 18px;"
                        "border : 1px solid #3ECEFE;"
                        "font-family : 微软雅⿊;"
                        "font-size : 14px;"
                        "font-style : normal;}");
    this->setTextMargins(33,0,0,0);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(searchImg);
    layout->addStretch();
    layout->addWidget(searchBtn);
    layout->setContentsMargins(11,0,2,0);

    connect(searchBtn, &QPushButton::clicked, this, &SearchLineEdit::searchBtnClicked);
    connect(this, &QLineEdit::returnPressed, this, &SearchLineEdit::searchBtnClicked);
}

void SearchLineEdit::searchBtnClicked()
{
    // 发射信息，让Player找服务器去搜索
    emit searchVideos(this->text());
}
