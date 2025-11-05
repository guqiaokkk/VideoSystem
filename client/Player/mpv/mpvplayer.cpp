#include "mpvplayer.h"
#include "util.h"

#include <QProcess>
#include <QDir>

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

    // 设置视频渲染窗⼝--将窗⼝的id告知给mpv
    // 如果设置了视频渲染窗⼝，就告知mpv，否则就不渲染视频画⾯和声⾳输出
    if(videoRenderWnd)
    {
        int64_t wid = videoRenderWnd->winId();
        mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);
    }
    else
    {
        // 此处不需要视频播放，让视频在后台加载成功即可
        // vo 表⽰视频输出 ao表⽰⾳频输出
        // vo null:表⽰禁⽌视频输出，视频不会被渲染到任何设备上
        // ao null：表⽰禁⽌⾳频输出，⾳频不会被播放到任何设备上
        mpv_set_option_string(mpv, "vo", "null");
        mpv_set_option_string(mpv, "ao", "null");
    }

    // 注册需要监控的时间
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_INT64);

    // 订阅 duration 属性变化
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);

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

QString MpvPlayer::getVideoFirstFrame(const QString &videoPath)
{
    // 使⽤ffmpeg⼯具获取视频⾸帧
    // 获取ffmpeg⼯具的路径
    QString ffmpegPath = QDir::currentPath() + "/ffmpeg/ffmpeg.exe";

    // 获取保存提取的⾸帧图⽚路径
    QString fristFrame = QDir::currentPath() + "/firstFrame.png";

    // 设置命令⾏参数
    QStringList cmd;
    cmd << "-ss"<<"00:00:00"
        <<"-i"<<videoPath
        <<"-vframes"<<"1"
        <<fristFrame;

    // 启动⼀个进程，⽤来管理ffmpeg⼯具
    QProcess ffmpegProgress;
    ffmpegProgress.start(ffmpegPath, cmd);

    // 等待进程完成：⽆限等待，直到进程结束
    if(!ffmpegProgress.waitForFinished(-1))
    {
        LOG()<<"ffmpeg 进程执⾏失败";
    }

    return fristFrame;
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

        else if(strcmp(eventPropery->name, "duration") == 0 && eventPropery->format == MPV_FORMAT_DOUBLE)
        {
            // 获取视频总时⻓
            int64_t duration = (int64_t)*(double *)eventPropery->data;
            emit durationChanged(duration);
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


