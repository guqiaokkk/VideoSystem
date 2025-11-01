#ifndef DATACENTER_H
#define DATACENTER_H

#include "data.h"
#include "../netclient/netclient.h"

#include <QObject>

namespace model{


class DataCenter : public QObject
{
    Q_OBJECT
public:
    // 获取DataCenter对象实例
    static DataCenter *getInstance();

    // 获取服务器地址
    const QString &getServerURL() const;

    // 获取所有分类
    const KindAndTag *getKindAndTagsClassPtr();

    // 获取sessionId
    const QString &getLogingSessionId()const;

    // 设置sessionId
    void setSeesionId(const QString &sessionId);


private:
    explicit DataCenter(QObject *parent = nullptr);


public:
    //定义所有异步请求方法
    void helloAsync();

    // 临时⽤⼾登录
    void loginTempUserAsync();

    // 获取所有视频列表
    void getAllVideoListAsync();

    // 解析响应中的所有视频对象，并放置VideoList中
    void setVideoList(const QJsonObject &videoListJsonObj);

    // 界⾯通过该指针将视频信息更新到界⾯
    VideoList *getVideoListPtr();

    // 获取分类视频列表
    void getAllVideoInKindAsync(int kindId);

    // 获取标签下的所有视频
    void getAllVideoInTagAsync(int tagId);

    // 根据搜索内容获取视频
    void getAllVideosBySearchTextAsync(const QString &searchText);

    // 下载图⽚
    void downloadPhotoAsync(const QString &photoFileId);

    // 下载视频
    void downloadVideoAsync(const QString &videoFileId);

    // 获取弹幕
    void getVideoBarrageAsync(const QString &videoId);
    // 解析弹幕数据
    void setBarragesData(const QJsonArray &barrageArray);
    // 获取弹幕信息
    QHash<int64_t, QList<BarrageInfo>> &getBarragesData();

signals:
    void helloDone();

    // 临时⽤⼾登录
    void loginTempUserDone();

    // 获取所有视频信息处理完毕
    void getAllVideoListDone();

    // 获取分类下所有视频信息处理完毕
    void getAllVideoInKindDone();

    // 获取标签下所有视频信息处理完毕
    void getAllVideoInTagDone();

    // 根据搜索内容获取视频处理完毕
    void getVideoBySearchTextDone();

    // 下载图⽚处理完毕
    // 每个VideoBox上都要下载视频封⾯和图⽚，导致下载图⽚完成信号会被绑定多次,⼀个信号被绑定多次时，当该信号触发时会被执⾏多次
    // 添加imageId参数，表明是某控件触发的下载图⽚请求，才处理该次图⽚下载的界⾯显⽰
    void downloadPhotoDone(const QString &imgId, QByteArray imgData);

    // 下载视频处理完毕
    void downloadVideoDone(const QString &videoFilePath, const QString &videoFileId);

    // 获取弹幕
    void getVideoBarrageDone(const QString &videoId);

private:
    static DataCenter *instance;

    // 服务器地址
    const QString serverURL = "http://127.0.0.1:8000";

    // 分类和标签实例指针
    KindAndTag *kindAndTags = nullptr;

    network::NetClient httpClient;

    // 当前客⼾端登录到服务器对应的登录会话 id
    QString loginSessionId = "";

    // 管理⾸⻚获取的视频信息: ⾸⻚中 分类视频列表、标签视频列表、所有视频列表、搜索视频 列表
    VideoList *videoListPtr = nullptr;

    // 弹幕信息: key为发送弹幕的时间, value为该时间下的弹幕，
    QHash<int64_t, QList<BarrageInfo>> barrages;
};



}


#endif // DATACENTER_H
