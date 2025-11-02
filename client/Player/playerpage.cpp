#include "playerpage.h"
#include "ui_playerpage.h"

#include "login.h"
#include "toast.h"
#include "util.h"
#include "bulletscreenitem.h"
#include "./model/datacenter.h"

#include <QShortcut>

PlayerPage::PlayerPage(const model::VideoInfo& videoInfo, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerPage)
    , videoInfo(videoInfo)
{
    ui->setupUi(this);

    // 实例化⾳量调节窗⼝指针
    volume = new Volume(this);

    // 实例化倍数播放窗⼝对象
    playSpeed = new PlaySpeed(this);

    //initBarrageArea();

    // 更新播放数
    likeCount = videoInfo.likeCount;

    mpvPlayer = new MpvPlayer(this, ui->screen);

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_ShowModal);

    // 播放按钮绑定空格快捷键
    QShortcut *shortcur = new QShortcut(ui->playBtn);
    QKeySequence keySequence(" ");
    shortcur->setKey(keySequence);
    connect(shortcur, &QShortcut::activated, this, [=](){
        ui->playBtn->animateClick();
    });



    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &PlayerPage::onQuitBtnClicked);

    // 显⽰⾳量调节窗⼝
    connect(ui->volumeBtn, &QPushButton::clicked, this, &PlayerPage::onVolumeBtnClicked);

    // 绑定倍数播放按钮信号槽
    connect(ui->speedBtn, &QPushButton::clicked, this, &PlayerPage::onSpeedBtnClicked);

    // 点赞
    connect(ui->likeImgBtn, &QPushButton::clicked, this, &PlayerPage::onLikeImageBtnClcked);

    // 播放
    connect(ui->playBtn, &QPushButton::clicked, this, &PlayerPage::onplayBtnClicked);

    // 设置倍数播放
    connect(playSpeed, &PlaySpeed::setPlaySpeed, this, &PlayerPage::onPlaySpeedChanged);

    // 设置⾳量
    connect(volume, &Volume::setVolume, this, &PlayerPage::setVolume);

    // 点击进度条，设置播放进度
    connect(ui->videoSlider, &PlaySlider::setPlayProgress, this, &PlayerPage::setPlayProgress);

    //  视频进度切换
    connect(mpvPlayer, &MpvPlayer::playPositionChanged, this, &PlayerPage::onPlayPositionChanged);
    // 所有视频分⽚播放结束信号槽绑定
    connect(mpvPlayer, &MpvPlayer::endOfPlaylist, this, &PlayerPage::onEndOfPlaylist);


    // 开启弹幕
    connect(ui->bulletScreenBtn, &QPushButton::clicked, this, &PlayerPage::onBulletScreenClicked);

    // 发送弹幕
    connect(ui->bulletScreenText, &BarrageEdit::sendBulletScreen, this, &PlayerPage::onSendBulletScreenBtnClicked);

    // 该视频是否被点赞过：需要从服务器拿到该视频被当前⽤⼾点赞信息, 根据是否点赞过设置界⾯点赞按钮的样式
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getIsLikeVideoDone, this, [=](const QString &videoId, bool isLike){
        if(videoId != videoInfo.videoId){
            return;
        }
        if(isLike)
        {
            ui->likeImgBtn->setStyleSheet("border-image : url(:/images/PlayPage/dianzan.png)");
        }
        else
        {
             ui->likeImgBtn->setStyleSheet("border-image : url(:/images/PlayPage/quxiaodianzan.png)");
        }
        this->isLike = isLike;
    });

    dataCenter->getIsLikeVideoAsync(videoInfo.videoId);
}

PlayerPage::~PlayerPage()
{
    delete ui;

}

void PlayerPage::mousePressEvent(QMouseEvent *event)
{
    // 获取鼠标按下时相对于窗口的位置,检测位置是否在窗口的head内部
    QPoint point = event->position().toPoint();
    if(ui->playHead->geometry().contains(point)){
        if(event->button() == Qt::LeftButton){
            // 计算鼠标按下之后的全局坐标和窗口左上角的相对偏移
            // 在整个移动过程中，该偏移不能发生改变
            dragPos = event->globalPosition().toPoint() - geometry().topLeft();
            return;
        }
    }

    QWidget::mousePressEvent(event);
}

void PlayerPage::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->playHead->geometry().contains(point)){
        if(event->buttons() == Qt::LeftButton){
            move(event->globalPosition().toPoint() - dragPos);

            // 移动弹幕窗口到播放窗口的head下方
            QPoint point = geometry().topLeft();   // 播放窗口的左上角
            point.setY(point.ry() + ui->playHead->height());
            barrageArea->move(point);
            return;
        }
    }

    QWidget::mousePressEvent(event);
}

void PlayerPage::onVolumeBtnClicked()
{
    //获取播放窗口左上角的坐标，并将其转换为基于屏幕的坐标
    // QPoint(0, 0)为PlayerPage窗⼝的左上⻆, mapToGlobal(QPoint(0, 0))将PlayerPage窗⼝左上⻆转换为全局坐标
    moveWindows(mapToGlobal(QPoint(0,0)));

    volume->show();
}

void PlayerPage::moveWindows(const QPoint &point)
{
    // point 已是基于屏幕的全局坐标，就是已经到playerpage的左上角了

    // QPoint(this->width() - volume->width()-13, 533)  QPoint为基于播放窗口playerpage的偏移量
    // 13是⾳量调节窗⼝右边界和 PlayerPage的右边界的距离,533是⾳量调节窗⼝距离PlayerPage上边界的距离
    // newPoint最终为⾳量调节窗⼝在PlayerPage中的位置转化的全局坐标
    QPoint newPoint = point + QPoint(this->width() - volume->width() -13, 533);
    volume->move(newPoint);


    // 倍速播放窗⼝位置调整
    newPoint = point + QPoint(this->width() - playSpeed->width() - 77, 563);
    playSpeed->move(newPoint);
}

void PlayerPage::startPlaying()
{
    initBarrageArea();

    // 加载弹幕数据
    loadBulletScreenData();

    updataVideoInfoUI();

    ui->videoSlider->setPlayStep(0);

    // 构造m3u8⽂件的url，然后交给mpv播放
    auto dataCenter = model::DataCenter::getInstance();
    QString videoUrl = dataCenter->getServerURL() + "/HttpService/downloadVideo?fileId=";
    videoUrl += videoInfo.videoFileId;
    mpvPlayer->startPlay(videoUrl);

    // 视频加载成功之后会⽴⻢播放，初始时先将其设置为暂停状态，当⽤⼾点击播放按钮之后再让 视频播放起来
    mpvPlayer->pause();

    // 点击播放时需要更新播放次数
    isUpdataPlayNum = false;
}

void PlayerPage::loadBulletScreenData()
{
    // 获取弹幕数据
    auto dataCenter = model::DataCenter::getInstance();
    bulletScreens = dataCenter->getBarragesData();
}

void PlayerPage::updataVideoInfoUI()
{
    // 设置视频标题
    ui->videoTittle->setText(videoInfo.videoTitle);

    // 设置⽤⼾昵称
    ui->userNickName->setText(videoInfo.nickName);

    // 设置视频上传时间
    ui->loadupTime->setText(videoInfo.videoUpTime);

    // 设置点赞数量
    ui->likeNum->setText(intToString(videoInfo.likeCount));

    // 设置播放数量
    ui->playNum->setText(intToString(videoInfo.playCount));

    // 设置视频播放时⻓
    QString curPlayTime = secondToTime(0);
    QString totalTime = secondToTime(videoInfo.videoDuration);
    ui->videoDuration->setText(curPlayTime + "/" + totalTime);

    // 修改视频描述
    ui->videoDesc->setText(videoInfo.videoDesc);
}


void PlayerPage::setUserIcon(QPixmap &userImg)
{
    // 设置⽤⼾图像
    ui->userAvatar->setIcon(QIcon(userImg));
}

void PlayerPage::updataPlayCount()
{
    // 如果播放次数已经更新了，则不重复更新
    if(isUpdataPlayNum){
        return;
    }

    // 更新界⾯上播放次数
    videoInfo.playCount++;
    ui->playNum->setText(intToString(videoInfo.playCount));

    // 更新DataCenter中视频列表中播放数: ⾸⻚视频列表 和 我的⻚⾯视频列表
    auto dataCenter = model::DataCenter::getInstance();

    // ⾸⻚视频列表
    auto videoList = dataCenter->getVideoListPtr();
    videoList->incrementPlayNum(videoInfo.videoId);

    // 我的⻚⾯视频列表--【待处理】

    // 更新服务器上该视频的播放数
    dataCenter->setPlayNumberAsync(videoInfo.videoId);
    isUpdataPlayNum = true;

    // 通知VideoBox更新VideoBox界⾯上的播放数
    emit increasePlayCount(videoInfo.videoId);
}

void PlayerPage::onSpeedBtnClicked()
{
    moveWindows(mapToGlobal(QPoint(0,0)));
    // 弹出倍速窗⼝
    playSpeed->show();
}

void PlayerPage::onLikeImageBtnClcked()
{
    // 检测当前⽤⼾视频为临时⽤⼾，临时⽤⼾需要先登录然后才能点赞 -todo
    if(false){
        Login* login = new Login();
        Toast::showMessage("先登录，登录完成之后才能点赞", login);;
    }

    isLike = !isLike;
    // 根据反转后的新状态来更新UI和点赞数
    if(isLike)
    {
        likeCount++;
        ui->likeImgBtn->setStyleSheet("border-image:url(:/images/PlayPage/dianzan.png)");
    }
    else
    {
        likeCount--;
        ui->likeImgBtn->setStyleSheet("border-image:url(:/images/PlayPage/quxiaodianzan.png)");
    }
    ui->likeNum->setText(intToString(likeCount));
}

void PlayerPage::onplayBtnClicked()
{
    isPlay = !isPlay;
    if(isPlay)
    {
        // 播放
        ui->playBtn->setStyleSheet("border-image : url(:/images/PlayPage/bofang.png)");
        mpvPlayer->play();
    }
    else
    {
        // 暂停
        ui->playBtn->setStyleSheet("border-image : url(:/images/PlayPage/zanting.png)");
        mpvPlayer->pause();
    }

    // 只要⽤⼾点击播放了视频，就更新播放次数
    updataPlayCount();
}

void PlayerPage::onPlaySpeedChanged(double speed)
{
    mpvPlayer->setPlaySpeed(speed);
}

void PlayerPage::setVolume(int volumeRatio)
{
    mpvPlayer->setVolume(volumeRatio);
}


// 此处10是测试视频时⻓，视频时⻓将来是从视频⽂件中获取
void PlayerPage::onPlayPositionChanged(int64_t playTime)
{
    QString curPlayTime = secondToTime(playTime);
    QString totalTime = secondToTime(videoInfo.videoDuration);
    ui->videoDuration->setText(curPlayTime + "/" + totalTime);

    // 修改进度条
    ui->videoSlider->setPlayStep((double)playTime/videoInfo.videoDuration);

    // 随着视频播放持续，要实时更新弹幕数据
    showBulletScreen();
}

void PlayerPage::onEndOfPlaylist()
{
    // 当视频播放完成之后，将播放按钮图标以及状态设置暂停
    isPlay = false;
    ui->playBtn->setStyleSheet("border-image : url(:/images/PlayPage/zanting.png);");

    // 重新点击播放按钮播放视频
    startPlaying();
}

QString PlayerPage::secondToTime(int64_t second)
{
    QString time;

    // ⼩时存在时才显⽰
    if(second/60/60)
    {
        time += QString::asprintf("%02lld:", second/60/60);
    }

    // 拼接上分和秒
    time += QString::asprintf("%02lld:%02lld", second/60, second%60);
    return time;
}

void PlayerPage::initBarrageArea()
{
    // 创建弹幕的显⽰区域对话框，该对话框没有边框，背景透明
    if(nullptr != barrageArea){
        return;
    }
    barrageArea = new QDialog(this);
    barrageArea->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    barrageArea->setAttribute(Qt::WA_TranslucentBackground);
    barrageArea->setMinimumSize(this->width(), 38*3);


    // 垂直布局器，添加到对话框中
    QVBoxLayout *layout = new QVBoxLayout(barrageArea);
    barrageArea->setLayout(layout);

    // 在弹幕区域添加⽤来显⽰三⾏弹幕的控件
    top = new QFrame(this);
    top->setFixedSize(this->width(), 38);

    middle = new QFrame(this);
    middle->setFixedSize(this->width(), 38);

    bottom = new QFrame(this);
    bottom->setFixedSize(this->width(), 38);

    layout->addWidget(top);
    layout->addWidget(middle);
    layout->addWidget(bottom);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    // 移动弹幕窗⼝到播放窗⼝的head底下
    QPoint point = mapToGlobal(QPoint(0, 0));
    point.setY(point.y() + ui->playHead->height());
    LOG()<<"弹幕位置："<<point;

    barrageArea->move(point);
    barrageArea->show();
}

void PlayerPage::showBulletScreen()
{
    // 如果打开关闭时，则不需要添加弹幕到界⾯
    if(!isStartBs){
        return;
    }

    // 获取当前playTime时间点的所有弹幕
    QList<model::BarrageInfo> bulletScreenList = bulletScreens.value(mpvPlayer->getCurPlayTime());

    // 显示弹幕
    int xTop, xMid, xBottom;
    xTop = xMid = xBottom = top->width();
    BulletScreenItem *bs = nullptr;

    for(int i = 0; i < bulletScreenList.size(); ++i)
    {
        model::BarrageInfo &bsInfo = bulletScreenList[i];
        if(i % 3 == 0)
        {
            //弹幕显示在第一行
            bs = new BulletScreenItem(top);
            bs->setBulletScreenText(bsInfo.text);

            // 按照最大幕字数计算当前弹幕时⻓
            int duration = 10000 * xTop / (double)(30 * 18 + 1450);

            bs->setBulletScreenAnimation(xTop, duration);

            xTop += bs->width() + 18*4;     // 同⼀⾏间隔4个汉字，18是每个字的像素⼤⼩
        }
        else if(i % 3 == 1)
        {
            //弹幕显示在第2行
            bs = new BulletScreenItem(middle);
            bs->setBulletScreenText(bsInfo.text);

            // 按照最大幕字数计算当前弹幕时⻓
            int duration = 10000 * xMid / (double)(30 * 18 + 1450);

            bs->setBulletScreenAnimation(xMid, duration);

            xMid += bs->width() + 18*4;     // 同⼀⾏间隔4个汉字，18是每个字的像素⼤⼩
        }
        else
        {
            //弹幕显示在第3行
            bs = new BulletScreenItem(bottom);
            bs->setBulletScreenText(bsInfo.text);

            // 按照最大幕字数计算当前弹幕时⻓
            int duration = 10000 * xBottom / (double)(30 * 18 + 1450);
            // 同⼀个时间点：第三⾏弹幕往后偏移2个字
            bs->setBulletScreenAnimation(xBottom + 2*18, duration);

            xBottom += bs->width() + 18*4;     // 同⼀⾏间隔4个汉字，18是每个字的像素⼤⼩
        }
        bs->startAnimation();
    }
}

void PlayerPage::setPlayProgress(double playRatio)
{
    // 更新播放时间
    int playTime = videoInfo.videoDuration * playRatio;
    mpvPlayer->setCurrentPlayPosition(playTime);
}

void PlayerPage::onBulletScreenClicked()
{
    isStartBs = !isStartBs;
    if(isStartBs){
        ui->bulletScreenBtn->setStyleSheet("border- image:url(:/images/PlayPage/danmu.png)");

        // 打开弹幕
        barrageArea->show();
    }
    else{
        ui->bulletScreenBtn->setStyleSheet("border- image:url(:/images/PlayPage/danmuguan.png)");

        // 关闭弹幕
        barrageArea->hide();
    }
}

void PlayerPage::onSendBulletScreenBtnClicked(const QString &text)
{
    // 如果弹幕是关闭的则⽆法发送弹幕
    if(!isStartBs){
        return;
    }

    BulletScreenItem *bs = new BulletScreenItem(top);
    QPixmap pixmap(":/images/homePage/touxiang.png");
    bs->setBulletScreenIcon(pixmap);
    bs->setBulletScreenText(text);
    int duration = 10000 * top->width() / (double)(30*18 + 1450);   //  10s / (30*18 + 1450)  =  duration / 它对应的起始位置（此处是top->width)
    bs->setBulletScreenAnimation(top->width(), duration);
    bs->startAnimation();

    // 将弹幕数据发送到服务器
    model::BarrageInfo barrageInfo;
    barrageInfo.playTime = mpvPlayer->getCurPlayTime();
    barrageInfo.text = text;
    barrageInfo.userId = videoInfo.userId;

    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->loadupBarragesAsync(videoInfo.videoId, barrageInfo);
}

void PlayerPage::onQuitBtnClicked()
{
    // 如果⽤⼾确实登录，并且已经点赞，将点赞数据上传⾄服务器. 临时⽤⼾不允许点赞-todo
    auto dataCenter = model::DataCenter::getInstance();
    if(likeCount != videoInfo.likeCount)
    {
        // 视频被点赞或者取消点赞了,同步服务器
        dataCenter->setLikeNumberAsync(videoInfo.videoId);

        // 设置DataCenter中视频点赞数据
        // videoInfo指向本来就是DataCenter中的videoId的视频信息，此处不需要单独修改
        // 更新⾸⻚视频列表 和 我的视频列表
        auto videoListPtr = dataCenter->getVideoListPtr();
        videoListPtr->updateLikeCount(videoInfo.videoId, likeCount);
        videoInfo.likeCount = likeCount;

        //myselfPage --- todo

        // 通知videoBox修改点赞信息
        emit updataLikeNum(likeCount);
    }
    this->close();
    this->deleteLater();
}


