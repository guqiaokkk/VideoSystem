#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include "volume.h"
#include "playspeed.h"
#include "mpv/mpvplayer.h"
#include "bulletscreenitem.h"
#include "./model/data.h"
#include "login.h"

#include <QWidget>
#include <QFrame>

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(const model::VideoInfo& videoInfo, QWidget *parent = nullptr);
    ~PlayerPage();


    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    // 加载视频
    void startPlaying();

    // 加载弹幕数据
    void loadBulletScreenData();

    // 设置视频信息
    void updataVideoInfoUI();

    // 设置⽤⼾头像
    void setUserIcon(QPixmap &userImg);

    // 更新播放次数
    void updataPlayCount();

private slots:
    // ⾳量调节
    void onVolumeBtnClicked();

    // 倍速播放
    void onSpeedBtnClicked();

    void onLikeImageBtnClcked(); // 点赞,TEST LOGIN

    // 播放视频
    void onplayBtnClicked();

    // 倍数播放
    void onPlaySpeedChanged(double speed);

    // ⾳量调节
    void setVolume(int volumeRatio);

    // 播放位置改变
    void onPlayPositionChanged(int64_t playTime);

    // 所有视频分⽚播放结束信号槽
    void onEndOfPlaylist();

    // 设置播放进度
    void setPlayProgress(double playRatio);

    // 弹幕开关控制
    void onBulletScreenClicked();

    // 发送弹幕
    void onSendBulletScreenBtnClicked(const QString &text);

    // 关闭按钮点击
    void onQuitBtnClicked();

    // ⽤⼾头像点击
    void onUserAvatarClicked();


private:
    // 移动窗⼝控件(音量和倍速)
    void moveWindows(const QPoint &point);

    // 将秒转换为：xx:xx:xx 格式时间
    QString secondToTime(int64_t second);

    // 弹幕区域布局
    void initBarrageArea();

    // 显⽰弹幕
    void showBulletScreen();

signals:
    // 新增播放数
    void increasePlayCount(const QString &videoId);

    // 更新点赞信号
    void updataLikeNum(int64_t likeCount);

private:
    Ui::PlayerPage *ui;

    QPoint dragPos;

    // ⾳量调节窗⼝指针
    Volume *volume;

    // 倍数播放窗⼝
    PlaySpeed *playSpeed;

    // 封装mpv库，控制播放视频
    MpvPlayer *mpvPlayer = nullptr;


    // 默认情况下暂停
    bool isPlay = false;



    // 弹幕相关信息
    QDialog *barrageArea = nullptr;
    QFrame *top;
    QFrame *middle;
    QFrame *bottom;


    // 是否开启弹幕，默认弹幕开启
    bool isStartBs = true;

    // 保存视频信息
    model::VideoInfo videoInfo;

    // key：视频的当前播放时间 : value: 当前播放时间下的所有弹幕数据
    QHash<int64_t, QList<model::BarrageInfo>> bulletScreens;

    // 是否更新播放次数
    bool isUpdataPlayNum = false;

    // 检测是否是否被点赞过，默认没有点赞过
    bool isLike = false;

    // 标记点赞数
    int64_t likeCount;

    // 登录窗⼝实例指针
    Login *login;
};

#endif // PLAYERPAGE_H
