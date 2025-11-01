#include "homepagewidget.h"
#include "ui_homepagewidget.h"

#include "videobox.h"

#include "util.h"

#include "./model/datacenter.h"

#include <QScrollBar>

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePageWidget)
{
    ui->setupUi(this);

    connectSignalAndSlot();

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
    // 启动时默认高亮“分类”按钮
    kindBtn->setStyleSheet("background-color:#F1FDFF; color:#3ECEFF");

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
    auto dataCenter = model::DataCenter::getInstance();
    // 视频信息更新到界⾯
    connect(dataCenter, &model::DataCenter::getAllVideoListDone, this, [=](){
        upDataVideoList();
    });

    // 绑定获取分类视频信号槽
    connect(dataCenter, &model::DataCenter::getAllVideoInKindDone, this, [=](){
        upDataVideoList();
    });

    // 绑定获取标签视频信号槽
    connect(dataCenter, &model::DataCenter::getAllVideoInTagDone, this, [=](){
        upDataVideoList();
    });

    // 点击搜索按钮，或者在搜索框输⼊回⻋
    connect(ui->search, &SearchLineEdit::searchVideos, this, &HomePageWidget::onSearchVideos);
    // 绑定搜索的信号槽
    connect(dataCenter, &model::DataCenter::getVideoBySearchTextDone, this, [=](){
        upDataVideoList();
    });

    // 滚动条位置检测信号槽
    connect(ui->videoScroll->verticalScrollBar(), &QScrollBar::valueChanged, this, &HomePageWidget::onScrollAreaValueChanged);

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
    // 每次重置时默认高亮“标签”按钮
    tag->setStyleSheet("background-color:#F1FDFF; color:#3ECEFF");

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
    // 防⽌同⼀个分类连续点击
    const QString kindText = clickedkindBtn->text();
    if(curKind == kindText){
        // 说明连续点击同⼀个分类
        return;
    }

    clickedkindBtn->setStyleSheet("background-color:#F1FDFF;" "color:#3ECEFF");

    // 清除之前的分类和之前选中的标签
    curKind = kindText;
    curTag = "";                // 只要点击分类之后，就需要将标签清空

    // 清除其他分类上的颜⾊
    QList<QPushButton*> kindBtns = ui->classifys->findChildren<QPushButton*>();
    for(auto kindbtn : kindBtns)
    {
        // 只要不是当前点击的按钮
        if(kindbtn != clickedkindBtn)
        {
            // 将包括“分类”在内的所有其他按钮全部恢复为标准的黑色字体
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

    // 先清除之前⻚⾯显⽰的旧视频
    clearLayoutVideos();

    // 分类视频列表
    videoListStyle = KindStyle;

    // 从服务器获取该分类中的所有视频，不考虑标签
    dataCenter->getAllVideoInKindAsync(kindAndTagPtr->getKindId(kindText));
}

void HomePageWidget::onTagBtnClicked(QPushButton *clickedtagBtn)
{
    QString tagText = clickedtagBtn->text();
    // 点击同⼀个标签
    if(curTag == tagText){
        return;
    }

    // 将被点击的标签按钮设置为⾼亮显⽰
    clickedtagBtn->setStyleSheet("background-color: #F1FDFF;" "color:#3ECEFF;");

    // 清空之前的标签颜⾊
    QList<QPushButton*> tagbtns = ui->labels->findChildren<QPushButton*>();
    for(auto tagbtn : tagbtns)
    {
        // 只要不是当前点击的按钮
        if(tagbtn != clickedtagBtn)
        {
            // 将包括“标签”在内的所有其他按钮全部恢复为标准的灰色字体
            tagbtn->setStyleSheet("color : #666666");

        }
    }

    // 先清除之前⻚⾯显⽰的旧视频
    clearLayoutVideos();

    // 获取该标签下的所有视频
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();
    auto kinds = kindAndTagPtr->getAllKinds();
    if(curKind.isEmpty()){
        curKind = kinds[0];
    }

    videoListStyle = TagStyle; // 标签视频列表

    dataCenter->getAllVideoInTagAsync(kindAndTagPtr->getTagId(curKind, curTag));
}

void HomePageWidget::onRefreshBtnClicked()
{
    // 先将界⾯上之前的视频内容清空
    clearLayoutVideos();

    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();

    // 到服务器获取视频数据
    switch (videoListStyle) {
    case AllStyle:
        dataCenter->getAllVideoListAsync();
        break;
    case KindStyle:
        dataCenter->getAllVideoInKindAsync(kindAndTagPtr->getKindId(curKind));
        break;
    case TagStyle:
        dataCenter->getAllVideoInTagAsync(kindAndTagPtr->getTagId(curKind, curTag));
        break;
    case SearchStyle:
        dataCenter->getAllVideosBySearchTextAsync(ui->search->text());
        break;
    default:
        LOG()<<"暂不⽀持的数据类型";
        break;
    }
}

void HomePageWidget::onTopBtnClicked()
{
    // 置顶
    ui->videoScroll->verticalScrollBar()->setValue(0);
}

void HomePageWidget::upDataVideoList()
{
    // 获取到所有视频的Id
    auto dataCenter = model::DataCenter::getInstance();
    auto videoIdList = dataCenter->getVideoListPtr()->getVideoList();
    LOG()<<"从服务器获取"<<videoIdList.size()<<"个视频";
    int videoIndex = ui->videoGLayout->count();

    for(int i = videoIndex; i < videoIdList.size(); ++i)
    {
        // 构建视频显⽰框，将视频信息更新到显⽰框上
        VideoBox *videoBox = new VideoBox(videoIdList[i]);

        // 将视频框更新到界⾯:⼀⾏显⽰4个视频框
        ui->videoGLayout->addWidget(videoBox, videoIndex / 4, videoIndex % 4);
        videoIndex++;
    }
    LOG()<<"添加到layout中视频个数："<<ui->videoGLayout->count();
}

void HomePageWidget::clearLayoutVideos()
{
    // 清空之前旧的视频信息
    LOG()<<"清空⾸⻚中旧视频个数:"<<ui->videoGLayout->count();
    QLayoutItem *videoItem = nullptr;
    while ((videoItem = ui->videoGLayout->takeAt(0)) != nullptr) {
        delete videoItem->widget();
        delete videoItem;
    }
    repaint();

    // 清空dataCenter中视频列表中内容
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getVideoListPtr()->clearVideoList();

    // 视频清空之后，将滚动条恢复到最上⾯
    ui->videoScroll->verticalScrollBar()->setValue(0);
}

void HomePageWidget::onSearchVideos(const QString &searchText)
{
    // 先清除之前⻚⾯显⽰的旧视频
    clearLayoutVideos();

    // 搜索视频列表
    videoListStyle = SearchStyle;
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getAllVideosBySearchTextAsync(searchText);
}

void HomePageWidget::onScrollAreaValueChanged(int value)
{
    // ⻚⾯上视频清空之后，滚动条的位置就为0了，会触发该消息
    // 因此当value为0时，即滚动条在最上⾯，不应该触发获取下⼀⻚视频操作
    if(value == 0){
        return;
    }

    // 如果该分类下的所有视频都获取到就不⽤到服务器获取
    auto dataCenter = model::DataCenter::getInstance();
    auto videoList = dataCenter->getVideoListPtr();
    if(videoList->getVideoTotalCount() == videoList->getVideoCount()){
        return;
    }

    // 当滚动条滚动到最下⾯时，⾃动获取下⼀⻚
    if(value == ui->videoScroll->verticalScrollBar()->maximum())
    {
        // 继续获取下⼀⻚的视频数据
        auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();

        // 到服务器获取视频数据
        switch (videoListStyle) {
        case AllStyle:
            dataCenter->getAllVideoListAsync();
            break;
        case KindStyle:
            dataCenter->getAllVideoInKindAsync(kindAndTagPtr->getKindId(curKind));
            break;
        case TagStyle:
            dataCenter->getAllVideoInTagAsync(kindAndTagPtr->getTagId(curKind, curTag));
            break;
        case SearchStyle:
            dataCenter->getAllVideosBySearchTextAsync(ui->search->text());
            break;
        default:
            LOG()<<"暂不⽀持的数据类型";
            break;
        }

    }
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

// 从服务器获取视频所有视频列表
void HomePageWidget::initVideos()
{
    // ⽹格中的元素默认向向左上⻆对⻬
    ui->videoGLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->videoScroll->verticalScrollBar()->setValue(0);

    // 默认获取所有视频列表
    videoListStyle = AllStyle;
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getAllVideoListAsync();
}
