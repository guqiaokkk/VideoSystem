#include "mpvplayer.h"
#include "util.h"

static void wakeup(void *ctx)
{
    MpvPlayer *mvpPlayer = (MpvPlayer *)ctx;
    emit mvpPlayer->mpvEvents();
}

MpvPlayer::MpvPlayer(QObject *parent, QWidget *videoRenderWnd)
    : QObject{parent}
{
    // 设置程序区域 . 区域设置决定了程序在运⾏时如何处理各种本地化相关操作，⽐如：⽇期格式、数字格式、货币符号等

    // LC_NUMERIC: 设置数字格式(包括⼩数点和千位分隔符) , C：表⽰使⽤C标准的默认区域设置
    std::setlocale(LC_NUMERIC, "C");

    // 创建mpv实例
    mpv = mpv_create();
    if(mpv == nullptr)
    {
        LOG()<<"创建mpv实例失败";
        throw std::runtime_error("can't create mpv instance!!!");
    }

    // 设置视频渲染窗⼝
    int64_t wid = videoRenderWnd->winId();
    mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);

    // 注册需要监控的时间
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_INT64);

    // 设置mpv内部事件触发时的回到函数wakeup , 通过应⽤程序的主事件循环处理mpv事件
    connect(this, &MpvPlayer::mpvEvents, this, &MpvPlayer::onMpvEvents, Qt::QueuedConnection);
    mpv_set_wakeup_callback(mpv, wakeup, this);

    // 初始化mpv实例
    if(mpv_initialize(mpv) < 0)
    {
        LOG()<<"初始化mpv失败";
        mpv_destroy(mpv);
        throw std::runtime_error("init mpv instance failed!!!");
    }
}

MpvPlayer::~MpvPlayer()
{
    // 释放mpv实例
    if(mpv)
    {
        mpv_terminate_destroy(mpv);
    }
}

void MpvPlayer::startPlay(const QString &videoPath)
{
    // 与mpv_command相同，但异步运⾏命令来避免阻塞，直到进程终⽌
    const QByteArray c_filename = videoPath.toUtf8();
    const char *args[] = {"loadfile", c_filename.data(), NULL};
    mpv_command_async(mpv, 0, args);
}

void MpvPlayer::play()
{
    // pause：设置播放器的暂停状态，可以设置为暂停或播放
    int s = 0;
    mpv_set_property_async(mpv, 0, "pause", MPV_FORMAT_FLAG, &s);
}

void MpvPlayer::pause()
{
    int s = 1;
    mpv_set_property_async(mpv, 0, "pause", MPV_FORMAT_FLAG, &s);
}

void MpvPlayer::setPlaySpeed(double speed)
{
    mpv_set_property_async(mpv, 0, "speed", MPV_FORMAT_DOUBLE, &speed);
}

void MpvPlayer::setMute(bool isMuted)
{
    int flag = isMuted ? 1 : 0;
    mpv_set_property_async(mpv, 0, "mute", MPV_FORMAT_FLAG, &flag);
}

void MpvPlayer::setVolume(int64_t volume)
{
    mpv_set_property_async(mpv, 0, "volume", MPV_FORMAT_INT64, &volume);
}

void MpvPlayer::setCurrentPlayPosition(int64_t seconds)
{
    mpv_set_property_async(mpv, 0, "time-pos", MPV_FORMAT_INT64, &seconds);
}

int64_t MpvPlayer::getCurPlayTime() const
{
    return curPlayTime;
}

void MpvPlayer::onMpvEvents()
{
    // 处理所有事件，直到事件队列为空
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if(event->event_id == MPV_EVENT_NONE)
        {
            break;
        }

        // 处理特定事件
        handleMpvEvent(event);
    }
}

void MpvPlayer::handleMpvEvent(mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE:
    {
        mpv_event_property *eventPropery = (mpv_event_property *)event->data;
        if(eventPropery->data == nullptr){
            break;
        }
        if(strcmp(eventPropery->name, "time-pos") == 0)
        {
            // 获取当前分⽚的起始时间
            double segmentStartTime = 0;
            mpv_get_property(mpv, "demuxer-start-time", MPV_FORMAT_DOUBLE, &segmentStartTime);

            // 获取当前分⽚内的播放时间
            double segmentCurrentTime = 0;
            mpv_get_property(mpv, "time-pos", MPV_FORMAT_DOUBLE, &segmentCurrentTime);

            // 全局时间 = 分⽚起始时间 + 分⽚内当前时间
            curPlayTime = (int64_t)segmentStartTime + segmentCurrentTime - 1;

            // 播放进度发⽣改变，发出信号，让界⾯更新进度条和时间
            emit playPositionChanged(curPlayTime);
        }
        break;
    }
    case MPV_EVENT_END_FILE:
    {
        mpv_event_end_file *endFile = (mpv_event_end_file*)event->data;
        if(endFile->reason == MPV_END_FILE_REASON_EOF){
            // 检查是否播放最后⼀个视频分⽚
            int64_t playlist_pos = -1;
            int64_t playlist_count = -1;
            mpv_get_property(mpv, "playlist-pos", MPV_FORMAT_INT64, &playlist_pos);
            mpv_get_property(mpv, "playlist-count", MPV_FORMAT_INT64, &playlist_count);

            // 综合判断条件
            if((playlist_count > 0) && (playlist_pos == playlist_count - 1))
            {
                LOG() << "所有视频分⽚播放完毕";
                emit endOfPlaylist();
            }
            else
            {
                LOG() << "单个分⽚播放结束";
            }
        }
        break;
    }

    case MPV_EVENT_SHUTDOWN:
    {
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
    }

    default:
        break;
    }
}


