#ifndef DATA_H
#define DATA_H

#include <QList>
#include <QHash>
#include <QString>
#include <qjsonobject.h>


namespace model{


/////////////////////////////////////////////////
/// 分类和标签
/////////////////////////////////////////////////
class KindAndTag
{
public:
    KindAndTag();

    // 获取所有分类
    const QList<QString> getAllKinds()const;

    // 获取⼀个分类下所有标签及id
    const QHash<QString, int> getTagsByKind(QString kind)const;

    // 获取分类的id
    int getKindId(QString kind)const;

    // 获取kind分类下包含的tag的id
    int getTagId(QString kind, QString tag)const;

private:
    // key为分类名称，value为分类Id
    QHash<QString, int> kindIds;

    // 外层QHash的key为分类名称
    // 内层QHash的key为标签名称，value为标签Id
    QHash<QString, QHash<QString, int>> tagIds;

    static int id;
};


/////////////////////////////////////////////////
/// 弹幕
/////////////////////////////////////////////////
class BarrageInfo
{
public:
    QString barrageId;  // 弹幕id
    QString userId;     // 发送弹幕⽤⼾
    int64_t playTime;   // 发送弹幕时当前播放时间
    QString text;       // 弹幕内容

    // 解析弹幕数据
    void loadBarrageInfo(QJsonObject &barrageJson);
};


/////////////////////////////////////////////////
/// 视频信息结构
/////////////////////////////////////////////////
class VideoInfo
{
public:
    QString videoId;            // 视频Id
    QString userId;             // ⽤⼾Id
    QString userAvatarId;       // ⽤⼾图像Id
    QString nickName;           // ⽤⼾昵称
    QString videoFileId;        // 视频⽂件Id
    QString photoFileId;        // 视频封⾯Id
    int64_t likeCount;          // 点赞量
    int64_t playCount;          // 播放量
    int64_t videoSize;          // 视频⼤⼩
    QString videoDesc;          // 视频描述信息
    QString videoTitle;         // 视频标题
    int64_t videoDuration;      // 持续时⻓
    QString videoUpTime;        // 视频上传时间

    // 通过 JSON 对象加载视频信息
    void loadVideoInfo(const QJsonObject &jsonObj);
};


/////////////////////////////////////////////////
// 视频列表-界⾯上显⽰需要
/////////////////////////////////////////////////
class VideoList
{
public:
    VideoList();

    // 设置或获取下⼀次要获取视频⻚号
    void setPageIndex(int pageIndex);
    int getPageIndex() const;

    // 获取视频列表中：实际视频个数
    int getVideoCount() const;

    // 设置或获取特定条件下(⽐如分类)总视频个数，视频审核⻚⾯⽤来计算分⻚器上总⻚数
    void setVideoTotalCount(int videoTotalCount);
    int getVideoTotalCount() const;

    // 往视频列表中添加视频
    void addVideo(const VideoInfo &videoInfo);

    // 获取排序后的视频列表
    const QList<VideoInfo> &getVideoList() const;

    // 增加播放量
    void incrementPlayNum(const QString &videoId);

    //更新点赞数
    void updateLikeCount(const QString &videoId, int64_t likeCount);

    // 将列表中的所有视频清空
    void clearVideoList();

    QList<VideoInfo> videoInfos;        // ⽬前从服务器获取下来的视频数据
    int64_t pageIndex;                  // ⻚⾯索引
    int64_t videototalCount;            // 该条件下所包含的所有视频的总数,⽐如：分类选择动物，该字段为动物分类下视频总数
                                        // ⽤视频总数和PAGE_COUNT能计算出该分类下总共有多少⻚视频
    const static int PAGE_COUNT = 20;   // 一页中包含的视频个数
};


} //end model

#endif // DATA_H
