#ifndef NETCLIENT_H
#define NETCLIENT_H

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

    // 下载视频
    void downloadVideo(const QString &videoFileId);

    // 获取弹幕
    void getVideoBarrage(const QString &videoId);

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
