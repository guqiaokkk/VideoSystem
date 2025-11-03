#include "datacenter.h"

#include "../util.h"
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

    if(videoListArray.size() == 0)
    {
        videoListPtr->setPageIndex(videoListPtr->getPageIndex()-1);
    }
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

void DataCenter::uploadPhotoAsync(const QByteArray &photoData)
{
    httpClient.uploadPhoto(photoData);
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

void DataCenter::getMyselfInfoAsync()
{
    // 不传 userId, 就是获取⾃⼰信息，服务端可以通过session拿到当前登录的⽤⼾id
    httpClient.getUserInfo("");
}

void DataCenter::getOtherUserInfoAsync(const QString &userId)
{
    // 传 userId, 就是获取其他⼈的信息
    httpClient.getUserInfo(userId);
}

void DataCenter::setAvatarAsync(const QString &fileId)
{
    httpClient.setAvatar(fileId);
}

void DataCenter::getUserVideoListAsync(const QString &userId, int pageIndex)
{
     httpClient.getUserVideoList(userId, pageIndex);
}

void DataCenter::deleteVideoAsync(const QString &videoId)
{
    httpClient.deleteVideo(videoId);
}


// 获取当前⽤⼾信息-当前⽤⼾：指当前使⽤播放平台的⽤⼾
void DataCenter::setMyselfInfo(const QJsonObject &myselfInfoObj)
{
    if(this->myselfInfo == nullptr)
    {
        this->myselfInfo = new UserInfo();
    }
    this->myselfInfo->loadUserInfo(myselfInfoObj);
}

const UserInfo *DataCenter::getMyselfInfo() const
{
    return myselfInfo;
}

// 获取其他⽤⼾信息-其他⽤⼾：指⾮当前登录⽤⼾，⼀般通过点击播放⻚⾯个⼈头像按钮，切换到上传者界⾯时获取
void DataCenter::setOtherUserInfo(const QJsonObject &otherUserInfoObj)
{
    if(this->otherUserInfo == nullptr)
    {
        this->otherUserInfo = new UserInfo();
    }
    this->otherUserInfo->loadUserInfo(otherUserInfoObj);
}

UserInfo *DataCenter::getOtherUserInfo()
{
    return otherUserInfo;
}

void DataCenter::setAvatar(const QString &fileId)
{
    myselfInfo->avatarFileId = fileId;
}

void DataCenter::setUserVideoList(const QJsonObject &videoListJson)
{
    getUserVideoList();

    // 解析视频信息
    QJsonArray videoListArray = videoListJson["videoList"].toArray();
    for(int i = 0; i < videoListArray.size(); ++i)
    {
        // 解析出单个视频信息
        QJsonObject videoInfoObj = videoListArray[i].toObject();
        VideoInfo videoInfo;
        videoInfo.loadVideoInfo(videoInfoObj);

        // 视频信息放置到⽤⼾视频列表中
        userVideoList->videoInfos.push_back(videoInfo);
    }

    // 设置视频总个数
    int videoTotalCount = videoListJson["totalCount"].toInt();
    LOG()<<"视频总个数："<<videoTotalCount;
    userVideoList->setVideoTotalCount(videoTotalCount);

    // 如果本次未获取到视频，说明后续已经没有视频了，⻚⾯索引不需要+1
    if(videoListArray.size() == 0)
    {
        userVideoList->setPageIndex(userVideoList->getPageIndex()-1);
    }
}

VideoList *DataCenter::getUserVideoList()
{
    if(userVideoList == nullptr)
    {
        userVideoList = new VideoList();
    }
    return userVideoList;
}





}// end model


