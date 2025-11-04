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

    // 初始化数据⽂件
    void initDataFile();

    // 存储数据到⽂件中
    void saveDataFile();

    // 从数据⽂件中加载数据到内存
    void loadDataFile();

    // 获取服务器地址
    const QString &getServerURL() const;

    // 获取所有分类
    const KindAndTag *getKindAndTagsClassPtr();

    // 获取sessionId
    const QString &getLogingSessionId()const;

    // 设置sessionId
    void setSeesionId(const QString &sessionId);

    // 解析响应中的所有视频对象，并放置VideoList中
    void setVideoList(const QJsonObject &videoListJsonObj);

    // 界⾯通过该指针将视频信息更新到界⾯
    VideoList *getVideoListPtr();

    // 解析弹幕数据
    void setBarragesData(const QJsonArray &barrageArray);
    // 获取弹幕信息
    QHash<int64_t, QList<BarrageInfo>> &getBarragesData();

    // 获取和设置当前用户个人信息
    void setMyselfInfo(const QJsonObject &myselfInfoObj);
    const UserInfo *getMyselfInfo() const;

    // 获取和设置其他用户的个人信息
    void setOtherUserInfo(const QJsonObject &otherUserInfoObj);
    UserInfo *getOtherUserInfo();

    // 构建临时⽤⼾信息
    void buildTempUserInfo();

    // 清除⽤⼾信息
    void clearUserInfo();

    // 修改⽤⼾头像id
    void setAvatar(const QString &fileId);

    // 获取和设置⽤⼾视频列表
    void setUserVideoList(const QJsonObject &videoListJson);
    VideoList *getUserVideoList();

private:
    explicit DataCenter(QObject *parent = nullptr);


public:
    //定义所有异步请求方法
    void helloAsync();

    // 临时⽤⼾登录
    void loginTempUserAsync();

    // 获取所有视频列表
    void getAllVideoListAsync();


    // 获取分类视频列表
    void getAllVideoInKindAsync(int kindId);

    // 获取标签下的所有视频
    void getAllVideoInTagAsync(int tagId);

    // 根据搜索内容获取视频
    void getAllVideosBySearchTextAsync(const QString &searchText);

    // 下载图⽚
    void downloadPhotoAsync(const QString &photoFileId);

    // 上传图⽚
    void uploadPhotoAsync(const QByteArray &photoData);

    // 下载视频
    void downloadVideoAsync(const QString &videoFileId);

    // 获取弹幕
    void getVideoBarrageAsync(const QString &videoId);


    // 更新播放次数
    void setPlayNumberAsync(const QString &videoId);

    // 检测⽤⼾是否点赞某视频
    void getIsLikeVideoAsync(const QString &videoId);

    // 更新点赞
    void setLikeNumberAsync(const QString &videoId);

    // 新增弹幕
    void loadupBarragesAsync(const QString &videoId, const BarrageInfo &barrageInfo);

    // 获取当前⽤⼾个⼈信息
    void getMyselfInfoAsync();

    // 获取其他⽤⼾信息
    void getOtherUserInfoAsync(const QString &userId);

    // 设置⽤⼾头像
    void setAvatarAsync(const QString &fileId);

    // 获取我的视频列表 userId为空获取当前⽤⼾视频列表，否则获取其他⽤⼾视频列表
    void getUserVideoListAsync(const QString &userId, int pageIndex);

    // 删除指定视频
    void deleteVideoAsync(const QString &videoId);

    // 关注⽤⼾
    void newAttentionAsync(const QString& userId);
    // 取消关注⽤⼾
    void delAttentionAsync(const QString& userId);

    // 获取短信验证码
    void getAuthcodeAsync(const QString &email);

    // 验证码登录
    void loginWithMessageAsync(const QString &email, const QString &authcode, const QString &authcodeId);

    // 账号密码登录
    void loginWithPasswordAsync(const QString &email, const QString &password);

    // 会话登录
    void loginSessionAsync();

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

    // 上传图⽚完成
    void uploadPhotoDone(const QString &fileId);

    // 下载视频处理完毕
    void downloadVideoDone(const QString &videoFilePath, const QString &videoFileId);

    // 获取弹幕
    void getVideoBarrageDone(const QString &videoId);

    // 检测是否对视频点赞成功
    void getIsLikeVideoDone(const QString &videoId, bool isLike);

    // 获取⽤⼾个⼈信息, 数据保存到 DataCenter 中
    void getMyselfInfoDone();
    // 获取其他⽤⼾信息，数据保存到 DataCenter 中
    void getOtherUserInfoDone();

    // 设置⽤⼾头像完成
    void setAvatarDone();

    // 获取指定⽤⼾的视频列表，数据保存在 DataCenter 中
    void getUserListVideoDone(const QString &userId);

    // 删除视频完成
    void deleteVideoDone(const QString &videoId);

    // 关注⽤⼾
    void newAttentionDone(const QString& userId);
    // 取消关注⽤⼾
    void delAttentionDone(const QString& userId);

    // 获取短信验证码完成
    void getAuthcodeDone(const QString &authcodeId);

    // 验证码登录完成
    void loginWithMessageDone();
    void loginWithMessageFailed(const QString &errorInfo);

    // 验证码登录完成
    void loginWithPasswordDone();
    void loginWithPasswordFailed(const QString &errorInfo);

    // 会话登录完成
    void loginWithSessionDone(bool isTempUser);
    void loginWithSessionFailed(const QString &errorInfo);

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

    // 保存当前⽤⼾个⼈信息
    UserInfo *myselfInfo = nullptr;
    // 保存其他⽤⼾的信息
    UserInfo *otherUserInfo = nullptr;

    // 保存指定⽤⼾视频列表：我的视频列表 或 其他⽤⼾视频列表
    VideoList *userVideoList = nullptr;
};



}


#endif // DATACENTER_H
