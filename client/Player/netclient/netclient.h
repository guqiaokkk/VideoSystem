#ifndef NETCLIENT_H
#define NETCLIENT_H

#include "../model/data.h"
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

namespace model{
class DataCenter;
}

namespace network{

class NetClient : public QObject
{
    Q_OBJECT
public:
    NetClient(model::DataCenter* dataCenter = nullptr);

    // 发送hello请求
    void hello();

    // ping请求
    void ping();

    // 临时⽤⼾登录请求
    void tempLogin();

    // 获取所有视频
    void getAllVideoList();

    // 获取分类下的所有视频
    void getAllVideosInKind(int kindId);

    // 获取标签下的所有视频
    void getAllVideosInTag(int tagId);

    // 根据搜索⽂本获取视频
    void getVideosBySearchText(const QString &searchText);

    // 下载图⽚
    void downloadPhoto(const QString &photoFileId);

    // 上传图⽚
    void uploadPhoto(const QByteArray &photoData, QWidget* wndPtr);

    // 下载视频
    void downloadVideo(const QString &videoFileId);

    // 上传视频
    void uploadVideo(const QString &videoPath);

    // 获取弹幕
    void getVideoBarrage(const QString &videoId);

    // 更新播放次数
    void setPlayNumber(const QString &videoId);

    // 检测是否点赞
    void getIsLikeVideo(const QString &videoId);

    // 更新点赞
    void setLikeNumber(const QString &videoId);

    // 新增弹幕
    void loadupBarrages(const QString &videoId, const model::BarrageInfo &barrageInfo);

    // 获取⽤⼾信息
    void getUserInfo(const QString &userId);

    // 设置⽤⼾头像
    void setAvatar(const QString &fileId);

    // 获取指定⽤⼾视频列表
    void getUserVideoList(const QString& userId, int pageIndex, const QString& whichPage);

    // 删除指定视频
    void deleteVideo(const QString& videoId);

    // 关注⽤⼾
    void newAttention(const QString& userId);
    // 取消关注⽤⼾
    void delAttention(const QString& userId);

    // 获取短信验证码
    void getAuthcode(const QString &email);

    // 验证码登录
    void loginWithMessage(const QString &email, const QString &authcode, const QString &authcodeId);

    // 账号密码登录
    void loginWithPassword(const QString& email, const QString& password);

    // 会话登录
    void loginSession();

    // 退出登录
    void logout();

    // 设置密码
    void setPassword(const QString &newPassword);

    // 设置⽤⼾昵称
    void setNickname(const QString& nickname);

    // 上传视频描述信息
    void uploadVideoDesc(const model::VideoDesc& videoDesc);

    // 获取状态视频列表
    void getStatusVideoList(int videoStatue, int pageIndex);

    // 审核视频
    void checkVideo(const QString& videoId, bool result);

    // 上架视频
    void putwayVideo(const QString &videoId);

    // 下架视频
    void discardVideo(const QString& videoId);

private:
    // ⽣成请求id
    static QString makeRequestId();

    // 封装发送请求的逻辑
    QNetworkReply *sendHttpRequest(const QString &resourcePath, QJsonObject &jsonBody);

    // 封装处理响应的逻辑(包括判定 HTTP 正确性, 反序列化, 判定业务上的正确性)
    // 通过输出型参数, 表⽰这次操作是成功还是失败, 以及失败的原因
    QJsonObject handleHttpResponse(QNetworkReply *httpResp, bool &ok, QString &reason);

private:
    QString HTTP_URL; // 服务器地址

    QNetworkAccessManager httpClient;

    model::DataCenter *dataCenter = nullptr;
};

}// end network

#endif // NETCLIENT_H
