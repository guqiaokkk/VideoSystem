#include "checktable.h"
#include "ui_checktable.h"

#include "util.h"

#include "checktableitem.h"
#include "./model/datacenter.h"
#include "toast.h"

CheckTable::CheckTable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CheckTable)
{
    ui->setupUi(this);
    ui->videoStatus->addItem("全部分类");
    ui->videoStatus->addItem("待审核");
    ui->videoStatus->addItem("审核通过");
    ui->videoStatus->addItem("审核驳回");
    ui->videoStatus->addItem("已下架");
    //ui->videoStatus->addItem("转码中");
    ui->videoStatus->setCurrentIndex(0);

    //使⽤正则表达式对视频⽤⼾id进⾏校验
    //eg:7d98-a3846253-0003
    QRegularExpression regExp("^[0-9a-f]{4}-[0-9a-f]{8}-[0-9a-f]{4}$");
    QValidator *validator = new QRegularExpressionValidator(regExp, this);
    // 将创建的验证器设置到ui->userIdEdit上
    ui->userIdEdit->setValidator(validator);


    // 重置按钮点击信号槽绑定
    connect(ui->resetBtn, &QPushButton::clicked, this, &CheckTable::onResetBtnClicked);

    // 查询按钮点击信号槽绑定
    connect(ui->queryBtn, &QPushButton::clicked, this, &CheckTable::onQueryBtnClicked);

    // 获取⽤⼾视频列表
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getUserListVideoDone, this, &CheckTable::updateCheckTable);

    // 默认显⽰审核⻚⾯
    //updateCheckTable();


    // 创建分⻚器并显⽰
    paginator = new Paginator(1, ui->PaginatorArea);
    paginator->move(0, 15);
    paginator->show();

    // 获取状态视频列表完成
    connect(dataCenter, &model::DataCenter::getStatusVideoListDone, this, [=](){
        updateCheckTable("", "checkPage");
    });
}


CheckTable::~CheckTable()
{
    delete ui;
}

void CheckTable::updateCheckTable(const QString &userId, const QString whichPage)
{
    if("checkPage" != whichPage){
        return;
    }

    // 清空旧视频内容
    QLayoutItem *item = nullptr;
    while ((item = ui->layout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

    // 获取⽤⼾视频列表
    auto dataCenter = model::DataCenter::getInstance();
    auto statusVideoList = dataCenter->getStatusVideoList();
    if(statusVideoList == nullptr)
    {
        return;
    }

    // 重置分⻚器
    auto videoList = statusVideoList->videoInfos;
    int videoCountOfPage = model::VideoList::PAGE_COUNT;
    if(page == 1)
    {
        resetPaginator((statusVideoList->getVideoTotalCount() + videoCountOfPage-1) / videoCountOfPage);
    }

    for(int i = 0; i < videoList.size(); ++i)
    {
        CheckTableItem *item = new CheckTableItem(this, videoList[i]);
        ui->layout->addWidget(item);
    }
}

void CheckTable::onResetBtnClicked()
{
    // 设置按钮样式
    ui->resetBtn->setStyleSheet("background-color:#3ECEFF;"
                                "border-radius:4px;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#FFFFFF;");

    ui->queryBtn->setStyleSheet("background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#222222;");

    // 清空⽤⼾id
    ui->userIdEdit->setText("");
    ui->videoStatus->setCurrentIndex(0);

    // 获取⽤⼾视频列表
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isAdminDisable())
    {
        Toast::showMessage("您已被禁⽌了，⽆法进⾏操作");
    }
    else
    {
        getVideoList(1);
    }

}

void CheckTable::onQueryBtnClicked()
{
    ui->queryBtn->setStyleSheet("background-color:#3ECEFF;"
                                "border-radius:4px;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#FFFFFF;");

    ui->resetBtn->setStyleSheet("background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#222222;");
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isAdminDisable())
    {
        Toast::showMessage("您已被禁⽌了，⽆法进⾏操作");
    }
    else
    {
        getVideoList(1);
    }
}

void CheckTable::getVideoList(int page)
{
    this->page = page;


    // 系统⻚⾯不需要保存所有视频，每次只保存⼀⻚，因此DataCenter中也只保存⼀⻚即可
    // 注意：此处不能调⽤视频列表的clearVideoList⽅法，因为该⽅法会将视频总数也清空的， 会影响分⻚器⻚数计算
    // 视频审核⻚⾯，视频列表中只保存⼀个⻚⾯的视频，将QList清空即可
    //dataCenter->getStatusVideoList()->clearVideoList();

    auto dataCenter = model::DataCenter::getInstance();
    auto videoListPtr = dataCenter->getStatusVideoList();
    videoListPtr->videoInfos.clear();

    // 优先按照⽤⼾Id获取视频
    QString userId = ui->userIdEdit->text();
    if(!userId.isEmpty())
    {
        // 获取指定⽤⼾视频
        dataCenter->getUserVideoListAsync(userId, page, "checkPage");
    }
    else
    {
        // 获取状态视频列表
        model::VideoStatus videoStatue = static_cast<model::VideoStatus>(ui->videoStatus->currentIndex());
        dataCenter->getStatusVideoListAsync(videoStatue, page);
    }
}

void CheckTable::resetPaginator(int pageCount)
{
    // 当重新获取视频列表后，每次获取结果的⻚⾯都不⼀样，分⻚器重新设置
    if(paginator){
        delete paginator;
    }

    paginator = new Paginator(pageCount, ui->PaginatorArea);
    paginator->move(0, 15);
    paginator->show();

    // 分⻚器信号
    connect(paginator, &Paginator::pageChanged, this, [=](int page){
        getVideoList(page);
    });
}
