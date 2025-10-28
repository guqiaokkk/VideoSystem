#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include "volume.h"
#include "playspeed.h"
#include "mpv/mpvplayer.h"
#include "bulletscreenitem.h"

#include <QWidget>
#include <QFrame>

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(QWidget *parent = nullptr);
    ~PlayerPage();


    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    // 加载视频
    void startPlaying(const QString &videoFilePath);

    // 加载弹幕数据
    void loadBulletScreenData();


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

    // 设置播放进度
    void setPlayProgress(double playRatio);

    // 弹幕开关控制
    void onBulletScreenClicked();

    // 发送弹幕
    void onSendBulletScreenBtnClicked(const QString &text);

private:
    // 移动窗⼝控件(音量和倍速)
    void moveWindows(const QPoint &point);

    // 将秒转换为：xx:xx:xx 格式时间
    QString secondToTime(int64_t second);

    // 弹幕区域布局
    void initBarrageArea();

    // 显⽰弹幕
    void showBulletScreen();

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

    // 当前播放时⻓
    int64_t playTime = 0;

    // 当前播放视频路径
    QString videoPath;

    // 弹幕相关信息
    QDialog *barrageArea = nullptr;
    QFrame *top;
    QFrame *middle;
    QFrame *bottom;

    // 临时保存弹幕数据
    QMap<int64_t, QList<BulletScreenInfo>> *bulletScreenLists;

    // 是否开启弹幕，默认弹幕开启
    bool isStartBs = true;
};

#endif // PLAYERPAGE_H
