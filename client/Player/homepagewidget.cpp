#include "homepagewidget.h"
#include "ui_homepagewidget.h"

#include "videobox.h"

#include "util.h"

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
    // 创建分类按钮
    QPushButton *kindBtn = buildSelectBtn(ui->classifys, "3ECEFF", "分类");
    ui->classifyHLayout->addWidget(kindBtn);

    // 创建分类
    QList<QString> kinds = {"历史", "美⻝", "游戏", "科技", "运动", "动物", "旅⾏", "电影"};
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

    // 分类和该分类下所有标签映射
    tags = { {"历史", {"中国史", "世界史", "历史⼈物", "艺术", "⽂化", "奇闻"}},
            {"美⻝", {"美⻝测评", "美⻝制作", "美⻝攻略", "美⻝记录", "探店", "⽔果", "海鲜"}},
            {"游戏", {"游戏攻略", "单机游戏", "电⼦竞技", "⼿机游戏", "⽹络游戏", "游戏赛事", "桌游棋牌"}},
            {"科技", {"数码", "软件应⽤", "智能家居", "⼿机", "电脑", "⼈⼯智能", "基础设施"}},
            {"运动", {"篮球", "⾜球", "乒乓球", "⽻⽑球", "健⾝", "竞技体育", "运动装备"}},
            {"动物", {"哈基米", "大狗嚼", "宠物知识", "动物资讯", "野⽣动物", "动物世界", "萌宠"}},
            {"旅⾏", {"旅游攻略", "旅⾏Vlog", "⾃驾游", "交通", "环球旅⾏", "露营", "野外⽣存"}},
            {"电影", {"电影解说", "电影推荐", "电影剪辑", "搞笑", "吐槽", "悬疑", "经典"}}};

    // 创建分类下的标签，默认显⽰第0个分类
    resetTags(tags[kinds[0]]);
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
    resetTags(tags[clickedkindBtn->text()]);
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
