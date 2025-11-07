#include "checktableitem.h"
#include "ui_checktableitem.h"
#include "util.h"
#include "confirmdialog.h"

CheckTableItem::CheckTableItem(QWidget *parent, model::VideoInfo videoInfo)
    : QWidget(parent)
    , ui(new Ui::CheckTableItem)
{
    ui->setupUi(this);

    initStyleSheet();

    // 设置⽤⼾视频Id、⽤⼾昵称、视频标题、审核⼈
    this->videoInfo = videoInfo;
    ui->videoUserId->setText(videoInfo.userId);
    ui->nickNameLabel->setText(videoInfo.nickName);
    ui->videoTitleLabel->setText(videoInfo.videoTitle);

    if(videoInfo.videoStatus == model::waitForChecking)
    {
        ui->checkerLabel->setText("--");
    }
    else
    {
        ui->checkerLabel->setText(videoInfo.checkerName);
    }

    // 设置状态和操作
    ui->operationBtn->move(1186,51);
    ui->operationBtn2->hide();

    switch (videoInfo.videoStatus) {
    case model::waitForChecking:
    {
        ui->statusBtn->setText("待审核");
        ui->operationBtn->setText("通过");
        ui->operationBtn->move(1156, 51);
        ui->operationBtn2->setText("驳回");
        ui->operationBtn2->show();
        break;
    }
    case model::putaway:
    {
        ui->statusBtn->setText("已审核");
        ui->operationBtn->setText("下架");
        break;
    }
    case model::discard:
    {
        ui->statusBtn->setText("已下架");
        ui->operationBtn->setText("上架");
        break;
    }
    case model::reject:
    {
        ui->statusBtn->setText("已驳回");
        ui->operationBtn->setText("--");
        break;
    }
    default:
        break;
    }

    // 设置按钮上样式
    ui->statusBtn->setStyleSheet(styleSheet[ui->statusBtn->text()]);
    ui->operationBtn->setStyleSheet(styleSheet[ui->operationBtn->text()]);
    ui->operationBtn2->setStyleSheet(styleSheet["驳回"]);

    // 设置视频封⾯
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->downloadPhotoAsync(videoInfo.photoFileId);
    connect(dataCenter, &model::DataCenter::downloadPhotoDone, this, [=](const QString &imgId, QByteArray imgData){
        if(videoInfo.photoFileId != imgId){
            return;
        }

        // 将图⽚更新到界⾯
        QPixmap pixmap;
        pixmap.loadFromData(imgData);
        ui->videoBtn->setIcon(QIcon(pixmap));
    });

    // 视频封⾯按钮点击
    connect(ui->videoBtn, &QPushButton::clicked, this, &CheckTableItem::onVideoBtnClicked);

    // 操作按钮点击
    connect(ui->operationBtn, &QPushButton::clicked, this, &CheckTableItem::onOperatorBtnClicked);

    // 点击驳回按钮
    connect(ui->operationBtn2, &QPushButton::clicked, this, &CheckTableItem::onOperatorBtn2Clicked);
}

CheckTableItem::~CheckTableItem()
{
    delete ui;
}

void CheckTableItem::onVideoBtnClicked()
{
    // 在进⾏视频审核时，还没有弹幕数据，此处也不需要加载弹幕数据，直接播放即可
    playerPage = new PlayerPage(videoInfo);

    // 设置⽤⼾图像
    if(videoInfo.userAvatarId.isEmpty())
    {
        // 加载默认图像，制作圆形头像并设置到界⾯
        userAvatar.loadFromData(loadFileToByteArray(":/images/myself/defaultAvatar.png" ));
        // 设置⽤⼾头像
        playerPage->setUserIcon(userAvatar);
    }
    else
    {
        // 获取⽤⼾头像
        auto dataCenter = model::DataCenter::getInstance();
        connect(dataCenter, &model::DataCenter::downloadPhotoDone, this, [=](const QString &imgId, QByteArray imgData){
            if(imgId != videoInfo.userAvatarId)
            {
                return;
            }

            // 将图⽚更新到界⾯
            // 设置⽤⼾头像
            userAvatar.loadFromData(imgData);
            playerPage->setUserIcon(userAvatar);
        });
        dataCenter->downloadPhotoAsync(videoInfo.userAvatarId);
    }

    // 显⽰播放窗⼝
    playerPage->show();
    // 开始播放视频
    playerPage->startPlaying();
}

void CheckTableItem::onOperatorBtnClicked()
{
    // 操作按钮，操作按钮上的⽂本可能是：通过(状态按钮：待审核)
    //                             上架(状态按钮：已下架)
    //                             下架(状态按钮：已审核)
    QString btnText = ui->operationBtn->text();
    auto dataCenter = model::DataCenter::getInstance();
    ConfirmDialog* confirmDlg = new ConfirmDialog();
    model::VideoStatus videoStatus = static_cast<model::VideoStatus>(videoInfo.videoStatus);

    if(btnText == "通过")
    {
        confirmDlg->setText("确定审核通过吗?");
        confirmDlg->exec();
        if(confirmDlg->getConfirm())
        {
            // 视频审核成功，更新界⾯UI 和 发送视频审核请求
            ui->statusBtn->setText("已审核");
            ui->operationBtn->setText("下架");
            ui->operationBtn->move(1186, 51);
            ui->operationBtn2->hide();
            videoStatus = model::putaway;
            dataCenter->checkVideoAsync(videoInfo.videoId, true);
        }
    }
    else if(btnText == "上架")
    {
        confirmDlg->setText("确定上架视频吗?");
        confirmDlg->exec();
        if(confirmDlg->getConfirm())
        {
            // 视频审核成功，更新界⾯UI 和 发送视频审核请求
            ui->statusBtn->setText("已审核");
            ui->operationBtn->setText("下架");
            ui->operationBtn->move(1186, 51);
            ui->operationBtn2->hide();
            videoStatus = model::putaway;
            dataCenter->putwayVideoAsync(videoInfo.videoId);
        }
    }
    else if(btnText == "下架")
    {
        confirmDlg->setText("确定下架视频吗?");
        confirmDlg->exec();
        if(confirmDlg->getConfirm())
        {
            // 视频审核成功，更新界⾯UI 和 发送视频审核请求
            ui->statusBtn->setText("已下架");
            ui->operationBtn->setText("上架");
            ui->operationBtn->move(1186, 51);
            ui->operationBtn2->hide();
            videoStatus = model::discard;
            dataCenter->discardVideoAsync(videoInfo.videoId);
        }
    }
    else if("--")
    {
        LOG()<<"视频已经下架，⽆需进⾏任何操作";
    }
    else
    {
        LOG()<<"未知操作";
    }

    // 修改按钮样式
    ui->statusBtn->setStyleSheet(styleSheet[ui->statusBtn->text()]);
    ui->operationBtn->setStyleSheet(styleSheet[ui->operationBtn->text()]);
    ui->operationBtn2->setStyleSheet(styleSheet[ui->operationBtn2->text()]);

    // 修改改CheckTableItem对应的视频信息
    auto myselfInfo = dataCenter->getMyselfInfo();
    ui->checkerLabel->setText(myselfInfo->nickname);

    videoInfo.videoStatus = videoStatus;
    videoInfo.checkerId = myselfInfo->userId;
    videoInfo.checkerName = myselfInfo->nickname;
    videoInfo.checkerAvatar = myselfInfo->avatarFileId;

    // 修改DataCenter中的视频审核信息
    auto statusVideoList = dataCenter->getStatusVideoList();
    statusVideoList->updateVideoCheckInfo(videoInfo.videoId, videoStatus, myselfInfo->nickname, myselfInfo->userId, myselfInfo->avatarFileId);

    delete confirmDlg;
}

void CheckTableItem::onOperatorBtn2Clicked()
{
    auto dataCenter = model::DataCenter::getInstance();
    ConfirmDialog* confirmDlg = new ConfirmDialog();
    model::VideoStatus videoStatus = static_cast<model::VideoStatus>(videoInfo.videoStatus);
    confirmDlg->setText("确认驳回视频吗?");
    confirmDlg->exec();
    if(confirmDlg->getConfirm())
    {
        ui->statusBtn->setText("已驳回");
        ui->operationBtn->setText("--");
        ui->operationBtn->move(1186, 51);
        ui->operationBtn2->hide();

        // 修改按钮样式
        ui->statusBtn->setStyleSheet(styleSheet[ui->statusBtn->text()]);
        ui->operationBtn->setStyleSheet(styleSheet[ui->operationBtn->text()]);
        ui->operationBtn2->setStyleSheet(styleSheet[ui->operationBtn2->text()]);

        // 修改改CheckTableItem对应的视频信息
        auto myselfInfo = dataCenter->getMyselfInfo();
        ui->checkerLabel->setText(myselfInfo->nickname);

        videoInfo.videoStatus = videoStatus;
        videoInfo.checkerId = myselfInfo->userId;
        videoInfo.checkerName = myselfInfo->nickname;
        videoInfo.checkerAvatar = myselfInfo->avatarFileId;

        // 修改DataCenter中的视频审核信息
        auto statusVideoList = dataCenter->getStatusVideoList();
        statusVideoList->updateVideoCheckInfo(videoInfo.videoId, model::VideoStatus::reject,
                                              myselfInfo->nickname, myselfInfo->userId, myselfInfo->avatarFileId);

        dataCenter->checkVideoAsync(videoInfo.videoId, false);
    }
    delete confirmDlg;
}

void CheckTableItem::initStyleSheet()
{
    styleSheet.insert("待审核", "#statusBtn{border:none;"
                                "background-color:#FFF0E6;"
                                "font-size:12px;"
                                "color:#FE964A;"
                                "border-radius:10px;}");
    styleSheet.insert("通过", "#operationBtn{border:none;"
                              "font-size:14px;"
                              "color:#3686FF;}");
    styleSheet.insert("驳回", "#operationBtn2{border:none;"
                              "font-size:14px;"
                              "color:#FD6A6A;}");
    styleSheet.insert("已审核", "#statusBtn{border:none;"
                                "background-color:#EBF3FF;"
                                "font-size:12px;"
                                "color:#3686FF;"
                                "border-radius:10px;}");
    styleSheet.insert("下架", "#operationBtn{border:none;"
                              "font-size:14px;"
                              "color:#FD6A6A;}");
    styleSheet.insert("已下架", "#statusBtn{border:none;"
                                "background-color:#FFF0F0;"
                                "font-size:12px;"
                                "color:#FD6A6A;"
                                "border-radius:10px;}");
    styleSheet.insert("上架", "#operationBtn{border:none;"
                              "font-size:14px;"
                              "color:#3ECEFF;}");
    styleSheet.insert("已驳回", "#statusBtn{border:none;"
                                "background-color:#FFF0E6;"
                                "font-size:12px;"
                                "color:#FE964A;"
                                "border-radius:10px;}");
    styleSheet.insert("--", "#operationBtn{border:none;"
                            "font-size:14px;"
                            "color:#222222;}");
}
