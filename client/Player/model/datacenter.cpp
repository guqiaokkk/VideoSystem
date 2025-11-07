#include "datacenter.h"

#include "../util.h"
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>

namespace model{

DataCenter *DataCenter::instance = nullptr;

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
    , httpClient(this)
{
    // 加载数据
    loadDataFile();

}


DataCenter *model::DataCenter::getInstance()
{
    if(instance == nullptr)
    {
        instance = new DataCenter();
    }
    return instance;
}

void DataCenter::initDataFile()
{
    // 构造出⽂件的路径, 使⽤ appData 存储⽂件
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = basePath + "/Player.json";
    LOG() << "filePath=" << filePath;

    // 检测路径是否存在
    QDir dir;
    if(!dir.exists(basePath))
    {
        dir.mkpath(basePath);
    }

    // 构造好⽂件路径之后, 把⽂件创建出来.
    // 写⽅式打开, 并且写⼊初始内容
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        LOG() << "打开⽂件失败!" << file.errorString();
        return;
    }

    // 打开成功, 写⼊初始内容
    QString data = "{\n\n}";
    file.write(data.toUtf8());
    file.close();
}

void DataCenter::saveDataFile()
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = basePath + "/Player.json";
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        LOG() << "打开⽂件失败!" << file.errorString();
        return;
    }

    // 按照 json 格式来写⼊数据.
    QJsonObject jsonObj;
    jsonObj["loginSessionId"] = loginSessionId;

    // ⽤⼾类型
    QJsonArray roleTypeArray;
    for(auto &idType : myselfInfo->roleType)
    {
        roleTypeArray.append(idType);
    }
    jsonObj["roleType"] = roleTypeArray;

    // ⾝份类型
    QJsonArray identityTypeArray;
    for(auto &idType : myselfInfo->identityType)
    {
        identityTypeArray.append(idType);
    }
    jsonObj["identityType"] = identityTypeArray;

    // 把 json 写⼊⽂件
    QJsonDocument jsonDoc(jsonObj);
    QString s = jsonDoc.toJson();
    file.write(s.toUtf8());

    // 关闭⽂件
    file.close();
}

// 加载⽂件, 在 DataCenter 被实例化的时候, 调⽤执⾏的
void DataCenter::loadDataFile()
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = basePath + "/Player.json";

    LOG() << "正在加载数据文件, 路径: " << filePath;

    // 判定⽂件是否存在, 不存在则初始化, 并创建出新的空⽩的 json ⽂件
    QFileInfo fileInfo(filePath);
    if(!fileInfo.exists())
    {
        initDataFile();
    }

    // 读⽅式打开⽂件
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        LOG() << "打开⽂件失败! " << file.errorString();
        return;
    }

    // 读取到⽂件内容, 解析为 JSON 对象
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    if(jsonDoc.isNull())
    {
        LOG() << "解析 JSON ⽂件失败! JSON ⽂件格式有错误!";
        file.close();
        return;
    }

    // 解析JSON⽂件，拿到session信息
    QJsonObject jsonObj = jsonDoc.object();
    this->loginSessionId = jsonObj["loginSessionId"].toString();

    if(myselfInfo == nullptr)
    {
        myselfInfo = new UserInfo();
    }
    // roleType
    QJsonArray roleTypeArray = jsonObj["roleType"].toArray();
    for(int i = 0; i < roleTypeArray.size(); ++i)
    {
        myselfInfo->roleType.append(roleTypeArray[i].toInt());
    }
    // identityType
    QJsonArray identityArray = jsonObj["identityType"].toArray();
    for(int i = 0; i < identityArray.size(); ++i)
    {
        myselfInfo->identityType.append(identityArray[i].toInt());
    }
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

void DataCenter::uploadPhotoAsync(const QByteArray &photoData, QWidget* wndPtr)
{
    httpClient.uploadPhoto(photoData, wndPtr);
}

void DataCenter::downloadVideoAsync(const QString &videoFileId)
{
    httpClient.downloadVideo(videoFileId);
}

void DataCenter::uploadVideoAsync(const QString &videoPath)
{
    httpClient.uploadVideo(videoPath);
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

void DataCenter::getUserVideoListAsync(const QString &userId, int pageIndex, const QString& whichPage)
{
     httpClient.getUserVideoList(userId, pageIndex, whichPage);
}

void DataCenter::deleteVideoAsync(const QString &videoId)
{
    httpClient.deleteVideo(videoId);
}

void DataCenter::newAttentionAsync(const QString &userId)
{
    httpClient.newAttention(userId);
}

void DataCenter::delAttentionAsync(const QString &userId)
{
    httpClient.delAttention(userId);
}

void DataCenter::getAuthcodeAsync(const QString &email)
{
    httpClient.getAuthcode(email);
}

void DataCenter::loginWithMessageAsync(const QString &email, const QString &authcode, const QString &authcodeId)
{
    httpClient.loginWithMessage(email, authcode, authcodeId);
}

void DataCenter::loginWithPasswordAsync(const QString &email, const QString &password)
{
    httpClient.loginWithPassword(email, password);
}

void DataCenter::loginSessionAsync()
{
    httpClient.loginSession();
}

void DataCenter::logoutAsync()
{
    httpClient.logout();
}

void DataCenter::setPasswordAysnc(const QString &password)
{
    httpClient.setPassword(password);
}

void DataCenter::setNicknameAsync(const QString &nickname)
{
    httpClient.setNickname(nickname);
}

void DataCenter::uploadVideoDescAsync(const VideoDesc &videoDesc)
{
    httpClient.uploadVideoDesc(videoDesc);
}

void DataCenter::getStatusVideoListAsync(int videoStatue, int pageIndex)
{
    httpClient.getStatusVideoList(videoStatue, pageIndex);
}

void DataCenter::checkVideoAsync(const QString &videoId, bool result)
{
    httpClient.checkVideo(videoId, result);
}

void DataCenter::putwayVideoAsync(const QString &videoId)
{
    httpClient.putwayVideo(videoId);
}

void DataCenter::discardVideoAsync(const QString &videoId)
{
    httpClient.discardVideo(videoId);
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

UserInfo *DataCenter::getMyselfInfo()
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

void DataCenter::buildTempUserInfo()
{
    if(this->myselfInfo == nullptr)
    {
        this->myselfInfo = new UserInfo();
    }

    // 临时⽤⼾没有⽤⼾信息，服务端权限受限，不需要到服务器获取
    // 直接将UserInfo设置为临时⽤⼾即可
    myselfInfo->buildTempUserInfo();
}

void DataCenter::clearUserInfo()
{
    if(myselfInfo)
    {
        delete myselfInfo;
        myselfInfo = nullptr;
    }
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

void DataCenter::setStatusVideoList(const QJsonObject &videoListObj)
{
    getUserVideoList();

    // 设置总⻚数
    QJsonArray videoListArray = videoListObj["videoList"].toArray();
    for(int i = 0; i < videoListArray.size(); ++i)
    {
        // 解析出单个视频信息
        QJsonObject videoInfoObj = videoListArray[i].toObject();
        VideoInfo videoInfo;
        videoInfo.loadVideoInfo(videoInfoObj);

        // 视频信息放置到⽤⼾视频列表中
        statusVideoList->videoInfos.push_back(videoInfo);
    }

    // 设置视频总个数
    int videoTotalCount = videoListObj["totalCount"].toInt();
    LOG()<<"视频总个数："<<videoTotalCount;
    statusVideoList->setVideoTotalCount(videoTotalCount);

    LOG()<<"状态视频列表总共有："<<statusVideoList->getVideoTotalCount()<<"个视频";
}

VideoList *DataCenter::getStatusVideoList()
{
    if(statusVideoList == nullptr)
    {
        statusVideoList = new VideoList();
    }
    return statusVideoList;
}





}// end model


