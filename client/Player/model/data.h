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
// 视频状态
enum VideoStatus{
    noStatus = 0, // ⽆状态
    waitForChecking, // 待审核
    putaway, // 审核通过 or 上架
    reject, // 审核驳回
    discard // 已下架
};


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
    int videoStatus;            // 视频状态
    QString checkerId;          // 审核者id
    QString checkerName;        // 审核者昵称
    QString checkerAvatar;      // 审核者⽤⼾头像id

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

    // 修改视频审核信息
    void updateVideoCheckInfo(const QString &videoId, VideoStatus videoStatus, const QString &nickname,
                              const QString &checkerId, const QString &checkerAvatarId);


    QList<VideoInfo> videoInfos;        // ⽬前从服务器获取下来的视频数据
    int64_t pageIndex;                  // ⻚⾯索引
    int64_t videototalCount;            // 该条件下所包含的所有视频的总数,⽐如：分类选择动物，该字段为动物分类下视频总数
                                        // ⽤视频总数和PAGE_COUNT能计算出该分类下总共有多少⻚视频
    const static int PAGE_COUNT = 20;   // 一页中包含的视频个数
};


/////////////////////////////////////////////////
/// 用户信息结构
/////////////////////////////////////////////////
enum RoleType
{
    SuperAdmin = 1,  // 超级管理员
    Admin,          // 普通管理员
    User,           // 普通⽤⼾
    TempUser        // 临时⽤⼾
};

//  身份类型
enum IdentityType
{
    CUser = 1,      // C 端⽤⼾
    BUser           // B 端⽤⼾
};

enum AdminStatus
{
    noAdminStatus = 0, // ⽆状态-获取所有管理员
    enable,            // 启⽤
    disable            // 禁⽤
};


class UserInfo
{
public:
    QString userId;             // 用户Id
    QString email;              // 邮箱
    QString nickname;           // 用户昵称
    QList<int> roleType;        // 角色类型：普通用户 普通管理员 超级管理员 临时用户
    QList<int> identityType;    // 身份类型：B端用户 C端用户
    int64_t likeCount;          // 点赞数
    int64_t playCount;          // 播放数
    int64_t followedCount;      // 关注数：当前用户关注其他用户的数量
    int64_t followerCount;      // 粉丝数：其他用户管理当前用户的数量
    int userStatus;             // 用户状态：管理员页面需要用到--启用和禁止
    bool isFollowing;           // 是否被关注: 主要在其他用户信息中使用
    QString userMemo;           // 用户备注信息：新增管理员时需要用到
    QString userCTime;          // 用户创建的时间
    QString avatarFileId;       // 用户头像id

    // 保存用户头像数据，该字段发送给服务器，只是客户端自己使用
    QByteArray userAvatarData;  // 用户头像数据

    // Json对象转换为UserInfo结构
    void loadUserInfo(const QJsonObject& userInfoJson);
    // 检测用户是否为B端用户
    bool isBUser()const;
    // 检测用户是否为临时用户
    bool isTempUser()const;
    // 构建临时用户个人信息
    void buildTempUserInfo();
    // 检测用户是否被禁用
    bool isAdminDisable()const;
};


/////////////////////////////////////////////////
/// 上传视频时描述信息
/////////////////////////////////////////////////
class VideoDesc
{
public:
    QString videoId;        // 视频⽂件id
    QString photoId;        // 视频封⾯id
    QString tittle;         // 视频标题
    QString desc;           // 视频描述
    QString kind;           // 视频分类
    QList<QString> tags;    // 视频标签
    int64_t duration;       // 视频持续时⻓
};

/////////////////////////////////////////////////
/// // 管理员信息
/////////////////////////////////////////////////
 class AdminInfo
{
 public:
    QString userId;                 // ⽤⼾Id
    QString email;                  // 邮箱号
    model::RoleType roleType;       // ⽤⼾⻆⾊
    QString nickName;               // ⽤⼾昵称
    AdminStatus userStatus;         // ⽤⼾状态
    QString remark;                 // 备注


    // QJsonObject 转 AdminInfo
    void loadAdminInfo(const QJsonObject& jsonObj);
};

// 管理员列表
class AdminList
{
public:
    // 添加管理员
    void addAdminInfo(const AdminInfo &adminInfo);
    // 设置状态
    void setAdminStatus(const QString& userId, AdminStatus adminStatus);

    QList<AdminInfo> adminList;         // 保存管理员信息
    int totalCount;                     // 系统中包含的管理员总的个数
    const static int PAGE_COUNT = 20;   // ⼀个⻚⾯显⽰的管理员信息
};

} //end model

#endif // DATA_H
