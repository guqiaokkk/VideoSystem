#include "myselfwidget.h"
#include "ui_myselfwidget.h"
#include "videobox.h"
#include "modifymyselfdialog.h"
#include "util.h"
#include "player.h"
#include "toast.h"
#include "confirmdialog.h"
#include "login.h"

#include "./model/data.h"
#include "./model/datacenter.h"

#include <QFileDialog>
#include <QScrollBar>
#include <QTimer>


////////////////////////////////// AttentionButton类 //////////////////////////////////
AttentionButton::AttentionButton(QWidget *parent) : QPushButton(parent)
{
    changesStatus(false);
}

void AttentionButton::changesStatus(bool isAttentionedStatus)
{
    this->isAttentionedStatus = isAttentionedStatus;
    if(isAttentionedStatus)
    {
        this->setText("已关注");
        this->setStyleSheet("QPushButton{"
                            "background-color: transparent;"
                            "color: #3ECEFE;"
                            "border-radius: 4px;"
                            "border: 1px solid #3ECEFE;"
                            "padding-left: 13px;"
                            "padding-right: 13px;}");
        this->setIconSize(QSize(24, 24));
        this->setIcon(QIcon(":/images/myself/guanzhu.png"));
    }
    else
    {
        this->setText("关注");
        this->setStyleSheet("QPushButton{"
                            "background-color: #3ECEFE;"
                            "color: #FFFFFF;"
                            "border-radius: 4px;"
                            "border: none;}");
        // 取消图标
        this->setIcon(QIcon());
    }
}

bool AttentionButton::isAttentioned() const
{
    return this->isAttentionedStatus;
}


////////////////////////////////// MyselfWidget类 //////////////////////////////////
MyselfWidget::MyselfWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyselfWidget)
{
    ui->setupUi(this);

    initUI();

    login = new Login(this);

    connectSignalAndSlots();


}

MyselfWidget::~MyselfWidget()
{
    delete ui;
}


//////////// myself////////////
void MyselfWidget::getMyselfInfo()
{
    auto dataCenter = model::DataCenter::getInstance();
    if(dataCenter->getMyselfInfo() == nullptr)
    {
        // 没有数据, 先请求数据
        dataCenter->getMyselfInfoAsync();
    }
    else
    {
        getMyselfInfoDone();
    }
}

void MyselfWidget::loadMyself()
{
    // 加载个⼈信息
    getMyselfInfo();

    // 加载个⼈视频列表
    // "" : 表⽰获取当前⽤⼾视频列表 1表⽰从获取第1⻚
    userId = "";


    // 切换到个⼈模式，即允许点击⽤⼾头像按钮修改个⼈头像
    ui->avatarBtn->changeMode(true);
    ui->avatarBtn->setEnabled(true);
}

//////////// otherself////////////
// 获取其他⽤⼾信息
void MyselfWidget::getOtherUserInfo(const QString &otherUserId)
{
    // 直接从⽹络获取, 不读取本地数据. 每次加载的其他⽤⼾可能都是不同的.
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getOtherUserInfoAsync(otherUserId);
}

void MyselfWidget::loadOtherUser(const QString &otherUserId)
{
    // 加载⽤⼾信息
    getOtherUserInfo(otherUserId);

    // 加载⽤⼾视频列表
    userId = otherUserId;
    getUserVideoList(otherUserId, 1);

    // 切换到其他⽤⼾模式
    ui->avatarBtn->changeMode(false);
    ui->avatarBtn->setEnabled(false); // 其他⽤⼾禁⽌修改头像
}

// 获取其他⽤⼾信息完成
void MyselfWidget::getOtherUserInfoDone()
{
    // 1. 界⾯控件显⽰隐藏处理
    // 默认情况是临时⽤⼾，所有显⽰信息控件已经隐藏了，此处需要重新显⽰下
    hideWidget(false);

    // 隐藏控件，禁⽌修改头像和点击昵称登录
    ui->settingBtn->hide();
    ui->quitBtn->hide();
    ui->uploadVideoBtn->hide();
    ui->avatarBtn->setEnabled(false);
    ui->nicknameBtn->setEnabled(false);

    // 2. 获取⽤⼾数据
    auto dataCenter = model::DataCenter::getInstance();
    auto otherUserInfo = dataCenter->getOtherUserInfo();

    // 3. 设置界⾯
    // 设置昵称、关注数、粉丝数、点赞数、播放数
    ui->nicknameBtn->setText(otherUserInfo->nickname);
    ui->attentionCountLabel->setText(intToString2(otherUserInfo->followedCount));
    ui->fansCountLabel->setText(intToString2(otherUserInfo->followerCount));
    ui->likeCountLabel->setText(intToString2(otherUserInfo->likeCount));
    ui->playCountLabel->setText(intToString2(otherUserInfo->playCount));

    // 设定关注按钮的状态, 并显⽰.
    ui->attentionBtn->changesStatus(otherUserInfo->isFollowing == 1);

    ui->myVideoLabel->setText("TA的视频");

    // 4. 设置头像
    if(otherUserInfo->avatarFileId.isEmpty())
    {
        // 该⽤⼾之前未上传头像，设置默认头像
        ui->avatarBtn->setIcon(QIcon(":/images/myself/default_avatar.png"));
    }
    else
    {
        dataCenter->downloadPhotoAsync(otherUserInfo->avatarFileId);
    }
}

void MyselfWidget::onAttentionBtnClicked()
{
    // 临时⽤⼾不能关注
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isTempUser())
    {
        Toast::showMessage("请先登陆或注册", login);
        return;
    }

    // 修改界⾯UI
    bool isAttentioned = ui->attentionBtn->isAttentioned();
    isAttentioned = !isAttentioned;
    ui->attentionBtn->changesStatus(isAttentioned);

    // 关注⻚⾯有效时才能关注
    auto otherUserInfo = dataCenter->getOtherUserInfo();
    if(otherUserInfo == nullptr)
    {
        LOG()<<"关注按钮点击时，指定⽤⼾信息不存在!!!";
        return;
    }

    if (isAttentioned)
    {
        dataCenter->newAttentionAsync(otherUserInfo->userId);
    }
    else
    {
        dataCenter->delAttentionAsync(otherUserInfo->userId);
    }
}

void MyselfWidget::newAttentionDone(const QString &userId)
{
    // 更新界⾯UI
    auto dataCenter = model::DataCenter::getInstance();
    auto otherUserInfo = dataCenter->getOtherUserInfo();
    int64_t followerCount = otherUserInfo->followerCount + 1;
    ui->fansCountLabel->setText(intToString2(followerCount));

    // 更新DataCenter
    otherUserInfo->followerCount = followerCount;
}

void MyselfWidget::delAttentionDone(const QString &userId)
{
    // 更新界⾯UI
    auto dataCenter = model::DataCenter::getInstance();
    auto otherUserInfo = dataCenter->getOtherUserInfo();
    int64_t followerCount = otherUserInfo->followerCount - 1;
    ui->fansCountLabel->setText(intToString2(followerCount));

    // 更新DataCenter
    otherUserInfo->followerCount = followerCount;
}

void MyselfWidget::logoutDone()
{
    // 清空界⾯上数据 以及 DataCenter中数据
    clearVideoList();
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getVideoListPtr()->clearVideoList();

    // 当前⽤⼾设置为临时⽤⼾
    dataCenter->buildTempUserInfo();
    loadMyself();

    // 主⻚中隐藏管理员⻚⾯切换按钮
    Player::getInstance()->showSystemPageBtn(false);
}



void MyselfWidget::initUI()
{
    ui->attentionBtn->hide();

    // 设置按钮绑定槽函数
    connect(ui->settingBtn, &QPushButton::clicked, this, &MyselfWidget::settingBtnClicked);
}

void MyselfWidget::uploadAvatarBtnClicked()
{
    // 0. 如果⽤⼾没有登陆，先要检测⽤⼾是否登陆
    auto dataCenter = model::DataCenter::getInstance();
    if(dataCenter->getMyselfInfo()->isTempUser())
    {
        Toast::showMessage("请先登陆，然后修改⽤⼾头像");
        return;
    }

    // 1.弹出对话框, 选择⽂件
    QString filename = QFileDialog::getOpenFileName(nullptr, "选择头像", "", "Image Files (*.jpg *.png)");
    if(filename.isEmpty()){
        LOG() << "取消选择头像";
        return;
    }

    // 2.读取文件到fileData
    QByteArray fileData = loadFileToByteArray(filename);
    if (fileData.isEmpty())
    {
        LOG() << "头像⽂件读取失败";
        return;
    }

    // 3.通过makeIcon将fileData转换为QIcon，并设为头像
    // 按钮的尺⼨为60*60，因此圆形按钮的半径为按钮⻓或宽的⼀半
    ui->avatarBtn->setIcon(makeIcon(fileData, ui->avatarBtn->width()/2));

    // 4. 上传图⽚⽂件到服务器
    dataCenter->uploadPhotoAsync(fileData, ui->avatarBtn);
}

void MyselfWidget::settingBtnClicked()
{

        ModifyMyselfDialog *dialog = new ModifyMyselfDialog(this);
        dialog->exec();
        delete dialog;

}

void MyselfWidget::uploadViewBtnClicked()
{
    // 1. 弹出打开⽂件对话框，让⽤⼾选择要上传的视频⽂件
    QString videoFilePath = QFileDialog::getOpenFileName(nullptr, "上传视频", "", "Videos (*.mp4 *.rmvb *.avi *.mov)");
    if(!videoFilePath.isEmpty())
    {
        // 视频⼤⼩限制，上限为4G
        QFileInfo fileInfo(videoFilePath);
        int64_t fileSize = fileInfo.size();
        if(fileSize > (int64_t)4 * 1024 * 1024 * 1024)
        {
            LOG()<<"视频⽂件必须⼩于4G";
            return;
        }

        // 上传视频⽂件
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->uploadVideoAsync(videoFilePath);

        // 切换到上传视频⻚⾯
        emit switchUploadVideoPage(UploadPage, videoFilePath);
    }
}

void MyselfWidget::onNicknameBtnClicked()
{
    // 如果当前⽤⼾恰好是临时⽤⼾，才显⽰登录界⾯
    auto dataCenter = model::DataCenter::getInstance();
    auto myself = dataCenter->getMyselfInfo();
    if(myself->isTempUser())
    {
        // 先清空界⾯旧数据，然后显式登录界⾯
        login->reset();
        login->show();
    }
}

void MyselfWidget::onSetNicknameDown(const QString &nickname)
{
    // 保证nicknameBtn的宽度和其字体宽度设置成⼀致
    ui->nicknameBtn->setText(nickname);
    ui->nicknameBtn->adjustSize();

    // 将设置按钮移动到昵称按钮之后，并间隔8个像素，y坐标不变
    QRect rect = ui->nicknameBtn->geometry();
    ui->settingBtn->move(rect.x() + rect.width() + 8, ui->settingBtn->geometry().y());

    auto dataCenter = model::DataCenter::getInstance();
    auto myself = dataCenter->getMyselfInfo();
    myself->nickname = nickname;
    LOG() << "修改昵称成功! nickname=" << nickname;
}

void MyselfWidget::onQuitBtnClicked()
{
    // 显⽰确认取消对话框
    ConfirmDialog confirmDiglog;
    confirmDiglog.setText("确定退出登录吗?");
    confirmDiglog.exec();

    // 如果选择确定则退出
    if(confirmDiglog.getConfirm())
    {
        // 发送退出登录请求
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->logoutAsync();
        LOG()<<"⽤⼾退出登录";
    }
}

void MyselfWidget::connectSignalAndSlots()
{
    // 头像按钮绑定槽函数
    connect(ui->avatarBtn, &QPushButton::clicked, this, &MyselfWidget::uploadAvatarBtnClicked);

    // 上传视频的绑定
    connect(ui->uploadVideoBtn, &QPushButton::clicked, this, &MyselfWidget::uploadViewBtnClicked);

    // 设置按钮点击
    connect(ui->settingBtn, &QPushButton::clicked, this, &MyselfWidget::settingBtnClicked);

    // 获取当前⽤⼾信息成功
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getMyselfInfoDone, this, &MyselfWidget::getMyselfInfoDone);

    // 获取⽤⼾头像信号槽绑定
    connect(dataCenter, &model::DataCenter::downloadPhotoDone, this, &MyselfWidget::getAvatarDone);

    // 上传图⽚
    connect(dataCenter, &model::DataCenter::uploadPhotoDone, this, &MyselfWidget::uploadAvatarDone1);

    // 修改⽤⼾头像Id成功
    connect(dataCenter, &model::DataCenter::setAvatarDone, this, &MyselfWidget::uploadAvatarDone2);

    // 获取视频列表
    connect(dataCenter, &model::DataCenter::getUserListVideoDone, this, &MyselfWidget::getUserVideoListDone);

    // 垂直滚动条滚动
    connect(ui->scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &MyselfWidget::onScrollAreaValueChanged);

    // 视频删除成功信号槽函数绑定
    connect(dataCenter, &model::DataCenter::deleteVideoDone, this, &MyselfWidget::deleteVideoDone);

    // 获取其他⽤⼾信息成功槽函数
    connect(dataCenter, &model::DataCenter::getOtherUserInfoDone, this, &MyselfWidget::getOtherUserInfoDone, Qt::UniqueConnection);

    // 关注按钮绑定槽函数
    connect(ui->attentionBtn, &AttentionButton::clicked, this, &MyselfWidget::onAttentionBtnClicked);

    // 关注请求响应成功
    connect(dataCenter, &model::DataCenter::newAttentionDone, this, &MyselfWidget::newAttentionDone);
    // 取消关注请求响应成功
    connect(dataCenter, &model::DataCenter::delAttentionDone, this, &MyselfWidget::delAttentionDone);

    // ⽤⼾昵称按钮点击
    connect(ui->nicknameBtn, &QPushButton::clicked, this, &MyselfWidget::onNicknameBtnClicked);

    // 退出登录按钮点击
    connect(ui->quitBtn, &QPushButton::clicked, this, &MyselfWidget::onQuitBtnClicked);

    // 登录成功
    connect(login, &Login::loginSuccess, this, [=](){
        //dataCenter->getMyselfInfoAsync();
        // 清空datacenter中保存的临时⽤⼾信息，从新加载当前⽤⼾的个⼈信息
        dataCenter->clearUserInfo();
        loadMyself();
    });

    // 退出登录成功
    connect(dataCenter, &model::DataCenter::logoutDone, this, &MyselfWidget::logoutDone);

    // 修改⽤⼾密码成功
    connect(dataCenter, &model::DataCenter::setPasswordDone, this, [=](){
       LOG() << "修改密码成功!";
    });

    // 设置昵称成功
    connect(dataCenter, &model::DataCenter::setNicknameDone, this, &MyselfWidget::onSetNicknameDown);
}

void MyselfWidget::hideWidget(bool isHide)
{
    // 临时⽤⼾需要隐藏界⾯上控件，⾮临时⽤⼾显⽰控件
    if(isHide)
    {
        ui->attentionBtn->hide();
        ui->attentionCountLabel->hide();
        ui->attentionLabel->hide();
        ui->fansLabel->hide();
        ui->fansCountLabel->hide();
        ui->likeLabel->hide();
        ui->likeCountLabel->hide();
        ui->playLabel->hide();
        ui->playCountLabel->hide();
        ui->settingBtn->hide();
        ui->quitBtn->hide();
        ui->uploadVideoBtn->hide();
        // scrollArea隐藏后，控件的位置仍旧保留
        QSizePolicy sizePolicy = ui->scrollArea->sizePolicy();
        sizePolicy.setRetainSizeWhenHidden(true);
        ui->scrollArea->setSizePolicy(sizePolicy);
        ui->scrollArea->hide();
        sizePolicy = ui->titleBar->sizePolicy();
        sizePolicy.setRetainSizeWhenHidden(true);
        ui->titleBar->setSizePolicy(sizePolicy);
        ui->titleBar->hide();
    }
    else
    {
        ui->attentionBtn->show();
        ui->attentionCountLabel->show();
        ui->attentionLabel->show();
        ui->fansLabel->show();
        ui->fansCountLabel->show();
        ui->likeLabel->show();
        ui->likeCountLabel->show();
        ui->playLabel->show();
        ui->playCountLabel->show();
        ui->settingBtn->show();
        ui->quitBtn->show();
        ui->uploadVideoBtn->show();
        ui->scrollArea->show();
        ui->titleBar->show();
    }
}

void MyselfWidget::getUserVideoList(const QString &userId, int pageIndex)
{
    // 如果获取的是第⼀⻚的视频时，需要将之前界⾯上的视频元素清空
    auto *dataCenter = model::DataCenter::getInstance();
    auto userVideoList = dataCenter->getUserVideoList();
    if(pageIndex == 1)
    {
        userVideoList->clearVideoList();
        clearVideoList();
    }
    dataCenter->getUserVideoListAsync(userId, pageIndex, "myPage");
    // page+1，滚动条向下滚动时就可以获取下⼀⻚视频
    userVideoList->setPageIndex(pageIndex + 1);
}

void MyselfWidget::clearVideoList()
{
    QLayoutItem* videoBoxWidget = nullptr;
    while((videoBoxWidget = ui->layout->takeAt(0)) != nullptr)
    {
        delete videoBoxWidget->widget();
        delete videoBoxWidget;
    }
}

void MyselfWidget::getUserVideoListDone(const QString &userId)
{
    auto *dataCenter = model::DataCenter::getInstance();
    auto *userVideoList = dataCenter->getUserVideoList();

    // 每⼀⾏显⽰ 4 个视频
    int rowCount = 4;
    for(int i = ui->layout->count(); i < userVideoList->getVideoCount(); ++i)
    {
        int row = i / rowCount;
        int col = i % rowCount;

        VideoBox *videobox = new VideoBox(userVideoList->videoInfos[i]);
        ui->layout->addWidget(videobox, row, col);

        // 如果是当前登录⽤⼾获取的视频列表，每个VideoBox都⽀持删除视频操作
        // userId为空，说明获取当前⽤⼾的视频列表，否则获取其他⽤⼾的视频列表
        if(userId == "")
        {
            videobox->showMoreBtn(true);
            connect(videobox, &VideoBox::deleteVideo, this, &MyselfWidget::deleteVideo);
        }
    }
    repaint();
}

void MyselfWidget::onScrollAreaValueChanged(int value)
{
    // 当value为0时，即滚动条在最上⾯，⽆需获取下⼀⻚视频
    if(value == 0)
    {
        return;
    }

    // 正在更新视频，再发出该信号时选择忽略，将以此更新的视频信息显⽰到界⾯后再处理下⼀次 更新
    if(ui->scrollArea->verticalScrollBar()->maximum() == value)
    {
        // 继续获取下⼀⻚的视频数据
        auto dataCenter = model::DataCenter::getInstance();
        auto userVideoListPtr = dataCenter->getUserVideoList();
        dataCenter->getUserVideoListAsync(userId, userVideoListPtr->getPageIndex(), "myPage");
        userVideoListPtr->setPageIndex(userVideoListPtr->getPageIndex()+1);
    }
}

void MyselfWidget::deleteVideo(const QString &videoId)
{
    auto *dataCenter = model::DataCenter::getInstance();
    dataCenter->deleteVideoAsync(videoId);
}

void MyselfWidget::deleteVideoDone(const QString &videoId)
{
    LOG() << "删除视频成功! videoId=" << videoId;
    // 刷新视频列表
    getUserVideoList("", 1);
}

void MyselfWidget::getMyselfInfoDone()
{
    // 1. 获取⽤⼾数据
    auto dataCenter = model::DataCenter::getInstance();
    auto *myself = dataCenter->getMyselfInfo();

    // 当前⽤⼾可能是普通⽤⼾、管理员、临时⽤⼾
    if(myself->isTempUser())
    {
        // 如果是临时⽤⼾
        hideWidget(true);
        ui->avatarBtn->setIcon(QIcon(":/image/myself/defaultAvatar.png"));
        ui->avatarBtn->setEnabled(false);   // 临时⽤⼾不允许修改头像
        ui->nicknameBtn->setText("点击登录");
        ui->nicknameBtn->adjustSize();
        ui->nicknameBtn->setEnabled(true);  // 允许点击昵称
        return;
    }
    else if(myself->isBUser())
    {
        // 如果是管理员，显⽰系统按钮，管理员可以后台操作
        Player *player = Player::getInstance();
        player->showSystemPageBtn(true);
    }

    // 普通⽤⼾和管理员都要恢复界⾯控件
    hideWidget(false);

    // 2. 界⾯更新
    // 设置昵称，根据昵称宽度调整设置按钮距离
    ui->nicknameBtn->setText(myself->nickname);
    ui->nicknameBtn->adjustSize();
    ui->nicknameBtn->setEnabled(false);

    // 根据昵称按钮⽂本⻓度移动设置按钮，即让设置按钮紧跟在昵称按钮之后  8是nicknameBtn和settingBtn间的间隙
    QRect rect = ui->nicknameBtn->geometry();
    ui->settingBtn->move(rect.x() + rect.width() + 8, ui->settingBtn->geometry().y());

    // 设置关注数、粉丝数、点赞数、播放数
    ui->attentionCountLabel->setText(intToString2(myself->followedCount));
    ui->fansCountLabel->setText(intToString2(myself->followerCount));
    ui->likeCountLabel->setText(intToString2(myself->likeCount));
    ui->playCountLabel->setText(intToString2(myself->playCount));

    // 3. 设置头像
    if(myself->avatarFileId.isEmpty())
    {
        // 设置默认头像
        ui->avatarBtn->setIcon(QIcon(":/image/myself/defaultAvatar.png"));
    }
    else
    {
        dataCenter->downloadPhotoAsync(myself->avatarFileId);
    }

    // 4. 其他：隐藏关注按钮、不能点击登录、允许修改头像
    ui->attentionBtn->hide();
    ui->avatarBtn->setEnabled(true);
    ui->myVideoLabel->setText("我的视频");

    getUserVideoList("", 1);
}

void MyselfWidget::getAvatarDone(const QString &fileId, const QByteArray &data)
{
    // 下列两种情况, 是互斥关系. 要么⽤⼾点击 "我的" 进⼊个⼈信息; 要么⽤⼾点击指定⽤⼾ 头像

    // 获取⾃⼰的头像
    auto myself = model::DataCenter::getInstance()->getMyselfInfo();
    if(myself != nullptr && myself->avatarFileId == fileId)
    {
        // 修改为设置圆形头像
        ui->avatarBtn->setIcon(makeIcon(data, ui->avatarBtn->width()/2));
    }

    // 获取他⼈头像
    auto otherUser = model::DataCenter::getInstance()->getOtherUserInfo();
    if(otherUser != nullptr && otherUser->avatarFileId == fileId)
    {
        ui->avatarBtn->setIcon(makeIcon(data, ui->avatarBtn->width()/2));
    }
}

void MyselfWidget::uploadAvatarDone1(const QString &fileId, QWidget* wndPtr)
{
    // 检测是否为上传的⽤⼾头像
    if(wndPtr != ui->avatarBtn)
    {
        return;
    }

    // 图⽚上传成功之后，将图⽚Id去修改服务器上⽤⼾头像id
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->setAvatarAsync(fileId);
}

void MyselfWidget::uploadAvatarDone2()
{
    // 重新通过⽤⼾头像 fildId 获取头像，头像获取成功会⾃动设置到界⾯
    auto dataCenter = model::DataCenter::getInstance();
    const auto *myself = dataCenter->getMyselfInfo();
    dataCenter->downloadPhotoAsync(myself->avatarFileId);
}


