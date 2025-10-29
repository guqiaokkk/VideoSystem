#include "homepagewidget.h"
#include "ui_homepagewidget.h"

#include "videobox.h"

#include "util.h"

#include "./model/datacenter.h"

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePageWidget)
{
    ui->setupUi(this);

    // 初始化分类和标签
    initKindsAndTags();

    // 初始化视频列表
    initVideos();

    // 刷新和置顶按钮
    initRefreshAndTop();
}

HomePageWidget::~HomePageWidget()
{
    delete ui;
}

void HomePageWidget::initKindsAndTags()
{
    // 到数据中⼼中获取所有分类数据
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();
    auto kinds = kindAndTagPtr->getAllKinds();

    // 创建分类按钮
    QPushButton *kindBtn = buildSelectBtn(ui->classifys, "3ECEFF", "分类");
    ui->classifyHLayout->addWidget(kindBtn);

    // 具体的分类按钮
    for(auto &kind : kinds)
    {
        QPushButton *kindbtn = buildSelectBtn(ui->classifys, "#222222", kind);
        ui->classifyHLayout->addWidget(kindbtn);

        // 按钮点击后修改按钮上颜⾊
        connect(kindbtn, &QPushButton::clicked, this, [=](){
            onKindBtnClicked(kindbtn);
        });
    }

    // 将按钮添加到分类的布局器中
    ui->classifyHLayout->setSpacing(8);

    // 获取分类下标签，默认显⽰第0个分类
    auto tags = kindAndTagPtr->getTagsByKind(kinds[0]).keys();
    resetTags(tags);
}

void HomePageWidget::connectSignalAndSlot()
{

}

QPushButton *HomePageWidget::buildSelectBtn(QWidget *parent, const QString &color, const QString &text)
{
    QPushButton *btn = new QPushButton(text, parent);
    btn->setMinimumHeight(26);
    btn->setFixedWidth(text.size()*16+18+18);
    btn->setStyleSheet("color:" + color);
    return btn;
}

void HomePageWidget::resetTags(const QList<QString> &tags)
{
    // 创建标签
    QPushButton *tag = buildSelectBtn(ui->labels, "#3ECEFF", "标签");
    ui->labelHLayout->addWidget(tag);

    // 创建具体标签按钮
    for(auto tagText : tags)
    {
        QPushButton *labelBtn = buildSelectBtn(ui->labels, "#666666", tagText);
        ui->labelHLayout->addWidget(labelBtn);
        connect(labelBtn, &QPushButton::clicked, this, [=](){
            onTagBtnClicked(labelBtn);
        });
    }

    ui->labelHLayout->setSpacing(4);
}

void HomePageWidget::onKindBtnClicked(QPushButton *clickedkindBtn)
{
    clickedkindBtn->setStyleSheet("background-color:#F1FDFF;" "color:#3ECEFF");

    // 清除其他分类上的颜⾊
    QList<QPushButton*> kindBtns = ui->classifys->findChildren<QPushButton*>();
    for(auto kindbtn : kindBtns)
    {
        if(clickedkindBtn != kindbtn){
            kindbtn->setStyleSheet("color : #222222");
        }
    }

    // 销毁之前的标签
    QList<QPushButton*> labels = ui->labels->findChildren<QPushButton*>();
    for(auto label : labels)
    {
        ui->labelHLayout->removeWidget(label);
        delete label;
    }

    // 根据当前选中分类，重新添加标签
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();
    resetTags(kindAndTagPtr->getTagsByKind(clickedkindBtn->text()).keys());
}

void HomePageWidget::onTagBtnClicked(QPushButton *clickedtagBtn)
{
    // 设置当前被点击标签按钮上颜⾊
    clickedtagBtn->setStyleSheet("background-color: #F1FDFF;" "color:#3ECEFF;");

    // 清空之前的标签颜⾊
    QList<QPushButton*> tagbtns = ui->labels->findChildren<QPushButton*>();
    for(auto tagbtn : tagbtns)
    {
        if(tagbtn != clickedtagBtn){
            tagbtn->setStyleSheet("color : #666666");
        }
    }
}

void HomePageWidget::onRefreshBtnClicked()
{
    LOG()<<"刷新按钮点击";
}

void HomePageWidget::onTopBtnClicked()
{

}

void HomePageWidget::initRefreshAndTop()
{
    // 刷新和置顶按钮
    QWidget *widget = new QWidget(this);
    widget->setFixedSize(42, 94);
    widget->setStyleSheet("QPushButton:hover{background-color:#666666}"
                          "QPushButton{" "background-color : #DDDDDD;"
                                         "border-radius : 21px;"
                                         "border : none;}");
    QVBoxLayout *layout = new QVBoxLayout(widget);

    // 刷新按钮
    QPushButton *refreshBtn = new QPushButton(widget);
    refreshBtn->setFixedSize(42,42);
    refreshBtn->setStyleSheet("border-image : url(:/images/homePage/shuaxin.png)");

    // 置顶按钮
    QPushButton *topBtn = new QPushButton(widget);
    topBtn->setFixedSize(42,42);
    topBtn->setStyleSheet("border-image : url(:/images/homePage/zhiding.png)");

    //加入布局
    layout->addWidget(refreshBtn);
    layout->addWidget(topBtn);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(10);

    widget->move(1278,618);
    widget->show();

    // 给两个按钮绑定槽函数
    connect(refreshBtn, &QPushButton::clicked, this, &HomePageWidget::onRefreshBtnClicked);
    connect(topBtn, &QPushButton::clicked, this, &HomePageWidget::onTopBtnClicked);
}

void HomePageWidget::initVideos()
{
    for(int i = 0; i < 16; ++i)
    {
        // 构建视频显⽰框，将视频信息更新到显⽰框上
        VideoBox *video = new VideoBox();

        // 将视频框更新到界⾯:⼀⾏显⽰4个视频框
        ui->videoGLayout->addWidget(video, i/4, i%4);
    }
}
