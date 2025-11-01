#include "videobox.h"
#include "ui_videobox.h"

#include "util.h"

#include <QDir>

VideoBox::VideoBox(model::VideoInfo videoInfo, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoBox)
    , videoInfo(videoInfo)
{
    ui->setupUi(this);

    // 默认隐藏删除按钮
    ui->delVideoBtn->hide();

    // 给VideoBox安装事件拦截器，让视频封⾯和标题控件的⿏标点击事件
    ui->imgBox->installEventFilter(this);
    ui->videoTitle->installEventFilter(this);

    // 设置视频信息到界⾯
    updataVideoInfoUI();

    playPage = new PlayerPage(videoInfo);

    // 获取视频封⾯图⽚成功
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::downloadPhotoDone, this, &VideoBox::getVideoImgDown);

    // 获取⽤⼾头像成功
    connect(dataCenter, &model::DataCenter::downloadPhotoDone, this, &VideoBox::getUserImgDone);

    // 获取弹幕数据成功
    connect(dataCenter, &model::DataCenter::getVideoBarrageDone, this, &VideoBox::getVideoBarrageSuccess);
}

VideoBox::~VideoBox()
{
    delete ui;
}

bool VideoBox::eventFilter(QObject *watched, QEvent *event)
{
    // 拦截视频封⾯和视频标题的⿏标点击事件
    if(ui->imgBox == watched || ui->videoTitle == watched)
    {
        if(QEvent::MouseButtonPress == event->type())
        {
            onPlayClicked();
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void VideoBox::updataVideoInfoUI()
{
    // 设置视频标题
    ui->videoTitle->setText(videoInfo.videoTitle);

    // 如果数字小于10000 直接显示数字 如果大于10000，以小数12345 将来1.2万
    // 设置点赞数
    ui->likeNum->setText(intToString(videoInfo.likeCount));

    // 设置播放量
    ui->playNum->setText(intToString(videoInfo.playCount));

    // 设置用户昵称
    ui->userNickName->setText(videoInfo.nickName);

    // 设置视频上传时间
    ui->loadupTime->setText(videoInfo.videoUpTime);

    // 设置视频持续时长
    // 12:45
    // 01:24:23
    setVideoDuration(videoInfo.videoDuration);

    // 设置视频封面
    setVideoImg(videoInfo.photoFileId);

    // 设置用户头像
    setUserIcon(videoInfo.userAvatarId);

}

void VideoBox::onPlayClicked()
{
    // 从服务器获取弹幕数据
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getVideoBarrageAsync(videoInfo.videoId);
}

void VideoBox::setVideoDuration(int64_t duration)
{
    QString time;

    // ⼩时存在时才显⽰
    if(duration/60/60)
    {
        time += QString::asprintf("%02lld:", duration/60/60);
    }

    // 拼接上分和秒
    time += QString::asprintf("%02lld:%02lld",duration/60,duration%60);
    ui->videoDuration->setText(time);
}

void VideoBox::setVideoImg(const QString &photoFileId)
{
    // 向服务器请求视频封⾯图⽚
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->downloadPhotoAsync(photoFileId);
}

void VideoBox::setUserIcon(const QString &userAvatarId)
{
    // 如果⽤⼾没有设置过头像，则使⽤默认头像
    if(userAvatarId.isEmpty()){
        ui->userIcon->setStyleSheet("border-image : url(:/images/myself/defaultAvatar.png)");
    }
    else
    {
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->downloadPhotoAsync(userAvatarId);
    }
}

void VideoBox::paintEvent(QPaintEvent *event)
{
    // true：控件在每次绘制时会⾃动使⽤调⾊板中的画刷来⾃动填充背景
    ui->imgBox->setAutoFillBackground(true);

    // 原始图⽚的尺⼨可能和界⾯尺⼨差距较⼤，对图⽚进⾏缩放
    // KeepAspectRatioByExpanding: 图⽚在缩放时会保持原始宽⾼⽐，但尽可能扩展以填满控件 SmoothTransformation: 采⽤平滑的差值算法对图⽚进⾏缩放，可以提⾼图⽚的视觉质量
    videoCoverImg = videoCoverImg.scaled(ui->imgBox->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // 重新设置画刷
    QPalette palette = ui->imgBox->palette();
    QBrush brush(videoCoverImg);
    palette.setBrush(QPalette::Window, brush);
    ui->imgBox->setPalette(palette);
}

void VideoBox::getVideoImgDown(const QString &imgId, QByteArray imgData)
{
    if(videoInfo.photoFileId != imgId){
        return;
    }

    // 将图⽚更新到界⾯
    videoCoverImg.loadFromData(imgData);
    repaint();
}

// 获取⽤⼾头像完成
void VideoBox::getUserImgDone(const QString &imgId, QByteArray imgData)
{
    // 不是当前⽤⼾在获取头像
    if(videoInfo.userAvatarId != imgId)
    {
        return;
    }

    // 制作圆形头像并设置到界⾯
    userAvatar = makeIcon(imgData, ui->userIcon->width() / 2).pixmap(ui->userIcon->size());
    ui->userIcon->setPixmap(userAvatar);
}

void VideoBox::getVideoBarrageSuccess(const QString &videoId)
{
    if(videoId != videoInfo.videoId){
        return;
    }

    playPage->show();
    // 从服务器下载M3U8⽂件并播放
    //dataCenter->downloadVideoAsync(videoInfo.videoFileId);
    //connect(dataCenter, &model::DataCenter::downloadVideoDone, this, [=](const QString &videoFilePath, const QString &videoFileId){
    //    if(videoInfo.videoFileId != videoFileId){
    //        return;
    //    }
    // 直接播放视频即可，mpv⾃⼰会去服务器下载m3u8⽂件
    //    playPage->startPlaying();
    //});

    // 直接播放视频即可，mpv⾃⼰会去服务器下载m3u8⽂件
    playPage->startPlaying();
}
