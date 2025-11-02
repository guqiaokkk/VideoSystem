#include "datacenter.h"

#include <QJsonArray>

namespace model{

DataCenter *DataCenter::instance = nullptr;

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
    , httpClient(this)
{}


DataCenter *model::DataCenter::getInstance()
{
    if(instance == nullptr)
    {
        instance = new DataCenter();
    }
    return instance;
}

const QString &DataCenter::getServerURL() const
{
    return serverURL;
}

const KindAndTag *DataCenter::getKindAndTagsClassPtr()
{
    if(kindAndTags == nullptr)
    {
        // 实例化
        kindAndTags = new KindAndTag();
    }
    return kindAndTags;
}

//////////////////////////////////////////////////////
/////////   session //////////////
//////////////////////////////////////////////////////
const QString &DataCenter::getLogingSessionId() const
{
    return loginSessionId;
}

void DataCenter::setSeesionId(const QString &sessionId)
{
    loginSessionId = sessionId;
}

void DataCenter::helloAsync()
{
    httpClient.hello();
}

void DataCenter::loginTempUserAsync()
{
    httpClient.tempLogin();
}


//////////////////////////////////////////////////////
/////////   videoListPtr //////////////
//////////////////////////////////////////////////////
// 获取所有视频列表
void DataCenter::getAllVideoListAsync()
{
    httpClient.getAllVideoList();
}

VideoList *DataCenter::getVideoListPtr()
{
    if(videoListPtr == nullptr)
    {
        videoListPtr = new VideoList();
    }
    return videoListPtr;
}

void DataCenter::setVideoList(const QJsonObject &videoListJsonObj)
{
    // 保证videoList对象先构造了
    getVideoListPtr();

    QJsonArray videoListArray = videoListJsonObj["videoList"].toArray();
    for(int i = 0; i < videoListArray.size(); ++i)
    {
        QJsonObject videoInfoObj = videoListArray[i].toObject();
        VideoInfo videoInfo;
        videoInfo.loadVideoInfo(videoInfoObj);
        videoListPtr->addVideo(videoInfo);
    }
    videoListPtr->setVideoTotalCount(videoListJsonObj["totalCount"].toInt());
}




/////////   kindId //////////////
void DataCenter::getAllVideoInKindAsync(int kindId)
{
    httpClient.getAllVideosInKind(kindId);
}
/////////   tagId //////////////
void DataCenter::getAllVideoInTagAsync(int tagId)
{
    httpClient.getAllVideosInTag(tagId);
}
/////////   search //////////////
void DataCenter::getAllVideosBySearchTextAsync(const QString &searchText)
{
    httpClient.getVideosBySearchText(searchText);
}
/////////  downloadPhoto ////////
void DataCenter::downloadPhotoAsync(const QString &photoFileId)
{
    httpClient.downloadPhoto(photoFileId);
}

void DataCenter::downloadVideoAsync(const QString &videoFileId)
{
    httpClient.downloadVideo(videoFileId);
}

void DataCenter::getVideoBarrageAsync(const QString &videoId)
{
    httpClient.getVideoBarrage(videoId);
}

void DataCenter::setBarragesData(const QJsonArray &barrageArray)
{
    // 清空之前旧弹幕数据
    barrages.clear();

    // 重新加载弹幕
    QList<BarrageInfo> barrageList;
    for(int i = 0; i < barrageArray.size(); ++i)
    {
        QJsonObject barrageObj = barrageArray[i].toObject();
        BarrageInfo barrageInfo;
        barrageInfo.loadBarrageInfo(barrageObj);

        if(!barrageList.isEmpty() && barrageList[0].playTime != barrageInfo.playTime)
        {
            // 该条弹幕是该时间下的第⼀条弹幕, 此时将上个时间点的弹幕先保存起来,然后处理新的弹幕
            barrages.insert(barrageList[0].playTime, barrageList);
            barrageList.clear();
        }
        barrageList.append(barrageInfo);
    }
    // 最后⼀组弹幕可能没有保存
    if(!barrageList.isEmpty())
    {
        barrages.insert(barrageList[0].playTime, barrageList);
    }
}

QHash<int64_t, QList<BarrageInfo> > &DataCenter::getBarragesData()
{
    return barrages;
}

void DataCenter::setPlayNumberAsync(const QString &videoId)
{
    httpClient.setPlayNumber(videoId);
}

void DataCenter::getIsLikeVideoAsync(const QString &videoId)
{
    httpClient.getIsLikeVideo(videoId);
}

void DataCenter::setLikeNumberAsync(const QString &videoId)
{
    httpClient.setLikeNumber(videoId);
}

void DataCenter::loadupBarragesAsync(const QString &videoId, const BarrageInfo &barrageInfo)
{
    httpClient.loadupBarrages(videoId, barrageInfo);
}



}// end model


