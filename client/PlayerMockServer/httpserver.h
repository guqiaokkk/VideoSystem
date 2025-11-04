#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>


//⻆⾊类型
enum RoleType{
    SuperAdmin = 1, // 超级管理员
    Admin = 2, // 管理员
    User = 3, // 普通⽤⼾
    TempUser = 4 // 临时⽤⼾
};

// 弹幕结构
class BarrageInfo
{
public:
    QString userId;     // 发送弹幕⽤⼾
    QString barrageId;  // 弹幕对应的视频id
    int64_t playTime;   // 发送弹幕时当前播放时间
    QString text;       // 弹幕内容

    BarrageInfo(const QString& userId = "", int64_t playTime = 0, const QString& text = "")
        : userId(userId)
        , playTime(playTime)
        , text(text)
    {}
};


class HttpServer : public QObject
{
    Q_OBJECT
public:
    bool init();
    static HttpServer *getInstance();


private:
    HttpServer();

    // 构造响应数据
    void buildResponseData();

    // 测试⽅法
    QHttpServerResponse hello(const QHttpServerRequest &req);

    // 临时⽤⼾登录响应
    QHttpServerResponse tempLogin(const QHttpServerRequest &req);

    // 获取所有视频列表
    QHttpServerResponse allVideoList(const QHttpServerRequest &req);

    // 获取分类视频列表
    QHttpServerResponse typeVideoList(const QHttpServerRequest &req);

    // 获取标签视频列
    QHttpServerResponse tagVideoList(const QHttpServerRequest &req);

    // 根据搜索⽂本获取视频内容
    QHttpServerResponse keyVideoList(const QHttpServerRequest &req);

    // 下载图⽚
    QHttpServerResponse downloadPhoto(const QHttpServerRequest &req);

    // 上传图⽚
    QHttpServerResponse uploadPhoto(const QHttpServerRequest& req);

    // 下载视频
    QHttpServerResponse downloadVideo(const QHttpServerRequest &req);
    // 下载视频分⽚
    QHttpServerResponse downVideoSegmentation(const QString &fileName);

    // 获取弹幕数据
    QHttpServerResponse getBarrage(const QHttpServerRequest &req);

    // 更新播放次数
    QHttpServerResponse setPlay(const QHttpServerRequest &req);

    // 检测是否对视频点赞
    QHttpServerResponse judgeLike(const QHttpServerRequest &req);

    // 更新点赞
    QHttpServerResponse setLike(const QHttpServerRequest &req);

    // 新增弹幕
    QHttpServerResponse newBarrage(const QHttpServerRequest &req);

    // 获取个⼈信息
    QHttpServerResponse getUserInfo(const QHttpServerRequest &req);

    // 设置⽤⼾头像
    QHttpServerResponse setAvatar(const QHttpServerRequest &req);

    // 获取视频列表
    QHttpServerResponse userVideoList(const QHttpServerRequest &req);

    // 删除视频
    QHttpServerResponse removeVideo(const QHttpServerRequest &req);

    // 关注⽤⼾
    QHttpServerResponse newAttention(const QHttpServerRequest &req);
    // 取消关注⽤⼾
    QHttpServerResponse delAttention(const QHttpServerRequest &req);

    // 获取短信验证码
    QHttpServerResponse getCode(const QHttpServerRequest &req);

    // 验证码登录
    QHttpServerResponse vcodeLogin(const QHttpServerRequest &req);

    // 账号密码登录
    QHttpServerResponse passwdLogin(const QHttpServerRequest &req);

    // 会话登录
    QHttpServerResponse loginSession(const QHttpServerRequest &req);

private:
    static HttpServer *instance;    // HttpServer的实例指针
    QHttpServer httpServer;         // Qt封装的QHttpServer对象

    // ⻆⾊类型
    RoleType roleType = TempUser;   // 默认为临时⽤⼾

    // 存放资源id和路径的对应关系
    QMap<QString, QString> idPathTable;

    // 保存视频的弹幕数据
    QList<BarrageInfo> barrages;
};

#endif // HTTPSERVER_H
