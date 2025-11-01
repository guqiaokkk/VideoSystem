#ifndef VIDEOBOX_H
#define VIDEOBOX_H

#include "playerpage.h"
#include "model/datacenter.h"

#include <QWidget>

namespace Ui {
class VideoBox;
}

class VideoBox : public QWidget
{
    Q_OBJECT

public:
    explicit VideoBox(model::VideoInfo videoInfo, QWidget *parent = nullptr);
    ~VideoBox();

    // 事件过滤
    bool eventFilter(QObject *watched, QEvent *event);

    // 设置视频信息到界⾯
    void updataVideoInfoUI();

private:
    void onPlayClicked();

    // 设置视频时⻓
    void setVideoDuration(int64_t duration);

    // 设置视频封⾯
    void setVideoImg(const QString &photoFileId);

    // 设置⽤⼾头像
    void setUserIcon(const QString &userAvatarId);

    // 重写paintEvent事件，避免图⽚平铺重叠
    void paintEvent(QPaintEvent *event);

private slots:
    // 视频封⾯图⽚获取成功
    void getVideoImgDown(const QString &imgId, QByteArray imgData);

    // 获取⽤⼾头像成功
    void getUserImgDone(const QString &imgId, QByteArray imgData);

    // 获取弹幕成功
    void getVideoBarrageSuccess(const QString &videoId);

private:
    Ui::VideoBox *ui;

    PlayerPage* playPage = nullptr; // 播放⻚⾯指针

    // 保存视频信息
    model::VideoInfo videoInfo;

    // 视频封面
    QPixmap videoCoverImg;

    // ⽤⼾图像
    QPixmap userAvatar;
};

#endif // VIDEOBOX_H
