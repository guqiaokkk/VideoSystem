#ifndef MPVPLAYER_H
#define MPVPLAYER_H

#include "client.h"

#include <QObject>
#include <QWidget>

class MpvPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MpvPlayer(QObject *parent = nullptr, QWidget *videoRenderWnd = nullptr);

    ~MpvPlayer();

    // 加载视频
    void startPlay(const QString& videoPath);

    // 播放和暂停
    void play();
    void pause();

    // 倍速播放
    void setPlaySpeed(double speed);

    // 静⾳模式
    void setMute(bool isMuted);

    // 设置⾳量⼤⼩
    void setVolume(int64_t volume);

    // 设置视频播放位置
    void setCurrentPlayPosition(int64_t seconds);

    // 获取当前播放时间
    int64_t getCurPlayTime()const;

private slots:
    void onMpvEvents();


signals:
    // 当订阅的事件发⽣时，触发该信号，利⽤Qt的信号槽机制处理
    void mpvEvents();

    // 通知界面更新当前播放事件
    void playPositionChanged(int64_t seconds);

    // 所有视频分⽚播放结束信号
    void endOfPlaylist();


private:
    // 处理mpv的事件
    void handleMpvEvent(mpv_event *event);

    // 统计视频播放的当前全局时间
    int64_t curPlayTime;

private:
    mpv_handle *mpv;
};

#endif // MPVPLAYER_H
