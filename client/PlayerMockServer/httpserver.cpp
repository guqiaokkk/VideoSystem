#include "httpserver.h"
#include "util.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

HttpServer *HttpServer::instance = nullptr;


// 该为私有方法，意味着外部代码不能直接 new HttpServer() 创建实例
// 只能通过特定的静态方法来获取实例
HttpServer::HttpServer()
{}

// 构造资源和id的对应关系
void HttpServer::buildResponseData()
{
    // 获取所有视频中：⽤⼾图像id、封⾯图像id与具体资源路径对应关系
    // 该模块下所有id均从10000开始
    int resourceId = 10000;
    for(int i = 0; i < 100; ++i)
    {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }

    // 通过分类获取所有视频中：⽤⼾图像id、封⾯图像id与具体资源路径对应关系
    // 该模块下所有id均从20000开始
    resourceId = 20000;
    for(int i = 0; i < 100; ++i)
    {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang1.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage1.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }

    // 通过标签获取所有视频中：⽤⼾图像id、封⾯图像id与具体资源路径对应关系
    // 该模块下所有id均从30000开始
    resourceId = 30000;
    for(int i = 0; i < 100; ++i)
    {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang2.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage2.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }

    // 通过关键字获取所有视频中：⽤⼾图像id、封⾯图像id与具体资源路径对应关系
    // 该模块下所有id均从40000开始
    resourceId = 40000;
    for(int i = 0; i < 100; ++i)
    {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang3.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage3.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }
}

bool HttpServer::init()
{
    // 构建资源和路径对应关系
    buildResponseData();

    // 绑定ip地址和端⼝号
    int64_t ret = httpServer.listen(QHostAddress::Any, 8000);

    // 配置路由
    httpServer.route("/hello", [=](const QHttpServerRequest &req){
        return this->hello(req);
    });

    // 临时登录
    httpServer.route("/HttpService/tempLogin", [=](const QHttpServerRequest &req){
        return this->tempLogin(req);
    });

    // 获取所有视频列表
    httpServer.route("/HttpService/allVideoList", [=](const QHttpServerRequest &req){
        return this->allVideoList(req);
    });

    // 获取分类视频列表
    httpServer.route("/HttpService/typeVideoList", [=](const QHttpServerRequest &req){
        return this->typeVideoList(req);
    });

    // 获取标签视频列表
    httpServer.route("/HttpService/tagVideoList", [=](const QHttpServerRequest &req){
        return this->tagVideoList(req);
    });

    // 根据搜索⽂本获取视频
    httpServer.route("/HttpService/keyVideoList", [=](const QHttpServerRequest &req){
        return this->keyVideoList(req);
    });

    // 下载图⽚
    httpServer.route("/HttpService/downloadPhoto", [=](const QHttpServerRequest &req){
        return this->downloadPhoto(req);
    });

    // 下载视频
    httpServer.route("/HttpService/downloadVideo", [=](const QHttpServerRequest &req){
        return this->downloadVideo(req);
    });
    // 模拟videos⽬录下视频⽂件下载
    // 客⼾端给mpv提供⼀个m3u8⽂件，mpv会从m3u8⽂件中获取到视频分⽚链接，下载到本地播放
    httpServer.route("/videos/", [=](const QString &request){
        return this->downVideoSegmentation(request);
    });

    // 获取弹幕
    httpServer.route("/HttpService/getBarrage", [=](const QHttpServerRequest &req){
        return this->getBarrage(req);
    });

    // 更新播放次数
    httpServer.route("/HttpService/setPlay", [=](const QHttpServerRequest &req){
        return this->setPlay(req);
    });

    // 检测是否点赞
    httpServer.route("/HttpService/judgeLike", [=](const QHttpServerRequest &req){
        return this->judgeLike(req);
    });

    // 更新点赞
    httpServer.route("/HttpService/setLike", [=](const QHttpServerRequest& req) {
        return this->setLike(req);
    });

    // 新增弹幕
    httpServer.route("/HttpService/newBarrage", [=](const QHttpServerRequest& req) {
        return this->newBarrage(req);
    });

    return ret == 8000;
}

HttpServer *HttpServer::getInstance()
{
    if(instance == nullptr)
    {
        instance = new HttpServer();
    }
    return instance;
}



QHttpServerResponse HttpServer::hello(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[hello] 收到 hello 请求, requestId = " <<jsonReq["requestId"].toString();

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["hello"] = "world";

    QJsonObject jsonResp;
    jsonResp["data"] = jsonBody;

    // 构造响应其他部分
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errmsg"] = "";
    jsonResp["errorCode"] = 0;

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}


// 临时⽤⼾登录响应
QHttpServerResponse HttpServer::tempLogin(const QHttpServerRequest &req)
{

    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[tempLogin] 收到 tempLogin 请求, requestId = " <<jsonReq["requestId"].toString();

    roleType = TempUser; // 临时⽤⼾

    // 构造响应体
    QJsonObject jsonBody;
    QString sessionId = QUuid::createUuid().toString();
    sessionId = sessionId.mid(25, 12);
    jsonBody["sessionId"] = sessionId;

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";
    jsonResp["data"] = jsonBody;

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

// 获取所有视频列表
QHttpServerResponse HttpServer::allVideoList(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[allVideoList] 收到 allVideoList 请求, requestId = " <<jsonReq["requestId"].toString();


    // 构造响应体
    QJsonObject jsonBody;
    int pageCount = jsonReq["pageCount"].toInt();
    jsonBody["totalCount"] = 1000;
    int videoId = 10000; int userId = 10000; int resourceId = 10000;

    QJsonArray videoLists;
    for(int i = 0; i < pageCount; ++i)
    {
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["nickname"] = "这是⼀个⽤⼾名";
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["photoFileId"] = QString::number(resourceId++);
        videoJsonObj["videoFileId"] = QString::number(resourceId++);
        videoJsonObj["likeCount"] = 1234; videoJsonObj["playCount"] = 3456;
        videoJsonObj["videoSize"] = 10240;
        videoJsonObj["videoDesc"] = "月华似练，笼中的羽翼被夜色收紧；花飞风追，少女的心愿随歌谣入梦~";
        videoJsonObj["videoTitle"] = "第一次去日本一个人怎么玩！七天自由行，好吃又好逛！";
        videoJsonObj["videoDuration"] = 10;
        videoJsonObj["videoUpTime"] = "9.16 12:28:58";
        videoLists.append(videoJsonObj);
    }
    jsonBody["videoList"] = videoLists;


    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";
    jsonResp["data"] = jsonBody;

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

QHttpServerResponse HttpServer::typeVideoList(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[typeVideoList] 收到 typeVideoList 请求, requestId = " <<jsonReq["requestId"].toString();
    LOG() << "kindId:"<<jsonReq["videoTypeId"].toString();

    // 构造响应体
    QJsonObject jsonBody;
    int pageCount = jsonReq["pageCount"].toInt();
    pageCount = 11;
    jsonBody["totalCount"] = 100;
    int videoId = 20000; int userId = 20000; int resourceId = 20000;

    QJsonArray videoLists;
    for(int i = 0; i < pageCount; ++i)
    {
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["nickname"] = "这是⼀个⽤⼾名";
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["photoFileId"] = QString::number(resourceId++);
        videoJsonObj["videoFileId"] = QString::number(resourceId++);
        videoJsonObj["likeCount"] = 1234; videoJsonObj["playCount"] = 3456;
        videoJsonObj["videoSize"] = 10240;
        videoJsonObj["videoDesc"] = "月华似练，笼中的羽翼被夜色收紧；花飞风追，少女的心愿随歌谣入梦~";
        videoJsonObj["videoTitle"] = "第一次去日本一个人怎么玩！七天自由行，好吃又好逛！";
        videoJsonObj["videoDuration"] = 10;
        videoJsonObj["videoUpTime"] = "9.16 12:28:58";
        videoLists.append(videoJsonObj);
    }
    jsonBody["videoList"] = videoLists;


    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";
    jsonResp["data"] = jsonBody;

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

QHttpServerResponse HttpServer::tagVideoList(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[tagVideoList] 收到 tagVideoList 请求, requestId = " <<jsonReq["requestId"].toString();

    // 构造响应体
    QJsonObject jsonBody;
    int pageCount = jsonReq["pageCount"].toInt();
    pageCount = 6;
    jsonBody["totalCount"] = pageCount;
    int videoId = 30000; int userId = 30000; int resourceId = 30000;

    QJsonArray videoLists;
    for(int i = 0; i < pageCount; ++i)
    {
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["nickname"] = "guqiao";
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["photoFileId"] = QString::number(resourceId++);
        videoJsonObj["videoFileId"] = QString::number(resourceId++);
        videoJsonObj["likeCount"] = 1234; videoJsonObj["playCount"] = 3456;
        videoJsonObj["videoSize"] = 10240;
        videoJsonObj["videoDesc"] = "月华似练，笼中的羽翼被夜色收紧；花飞风追，少女的心愿随歌谣入梦~";
        videoJsonObj["videoTitle"] = "第一次去日本一个人怎么玩！七天自由行，好吃又好逛！";
        videoJsonObj["videoDuration"] = 10;
        videoJsonObj["videoUpTime"] = "9.16 12:28:58";
        videoLists.append(videoJsonObj);
    }
    jsonBody["videoList"] = videoLists;


    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";
    jsonResp["data"] = jsonBody;

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

QHttpServerResponse HttpServer::keyVideoList(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[keyVideoList] 收到 keyVideoList 请求, requestId = " <<jsonReq["requestId"].toString();

    // 构造响应体
    QJsonObject jsonBody;
    int pageCount = jsonReq["pageCount"].toInt();
    pageCount = 1;
    jsonBody["totalCount"] = pageCount;
    int videoId = 40000; int userId = 40000; int resourceId = 40000;

    QJsonArray videoLists;
    for(int i = 0; i < pageCount; ++i)
    {
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["nickname"] = "guqiao";
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["photoFileId"] = QString::number(resourceId++);
        videoJsonObj["videoFileId"] = QString::number(resourceId++);
        videoJsonObj["likeCount"] = 1234; videoJsonObj["playCount"] = 3456;
        videoJsonObj["videoSize"] = 10240;
        videoJsonObj["videoDesc"] = "月华似练，笼中的羽翼被夜色收紧；花飞风追，少女的心愿随歌谣入梦~";
        videoJsonObj["videoTitle"] = "第一次去日本一个人怎么玩！七天自由行，好吃又好逛！";
        videoJsonObj["videoDuration"] = 10;
        videoJsonObj["videoUpTime"] = "9.16 12:28:58";
        videoLists.append(videoJsonObj);
    }
    jsonBody["videoList"] = videoLists;


    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";
    jsonResp["data"] = jsonBody;

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

QHttpServerResponse HttpServer::downloadPhoto(const QHttpServerRequest &req)
{
    // 解析查询字符串
    QUrlQuery query(req.url());
    QString requestId = query.queryItemValue("requestId");
    QString fileId = query.queryItemValue("fileId");
    LOG() << "[downloadPhoto] 收到 downloadPhoto 请求, requestId=" << requestId;

    // 构造图⽚路径
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cdUp();
    QString imgPath = dir.absolutePath();
    imgPath += idPathTable[fileId];
    LOG()<<"图⽚ID："<<fileId<<"--"<<imgPath;

    // 读取图⽚数据
    QByteArray imgData = loadFileToByteArray(imgPath);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(imgData, QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/octet-stream");
    return httpResp;

}

// 下载M3U8⽂件
QHttpServerResponse HttpServer::downloadVideo(const QHttpServerRequest &req)
{
    // 解析查询字符串
    QUrlQuery query(req.url());
    QString requestId = query.queryItemValue("requestId");
    QString fileId = query.queryItemValue("fileId");
    LOG() << "[downloadVideo] 收到 downloadVideo 请求, requestId=" << requestId;

    // 构造图⽚路径
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cdUp();
    QString videogPath = dir.absolutePath();
    videogPath += idPathTable[fileId];
    LOG()<<"视频id："<<fileId<<"--"<<videogPath;

    // 读取图⽚数据
    QByteArray videogData = loadFileToByteArray(videogPath);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(videogData, QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/octet-stream");
    return httpResp;
}

QHttpServerResponse HttpServer::downVideoSegmentation(const QString &fileName)
{
    // 构造视频分⽚⽂件路径
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cdUp();
    QString filePath = dir.absolutePath();
    filePath +=  "/videos/" + fileName;

    // 读取分⽚⽂件数据
    QByteArray fileData = loadFileToByteArray(filePath);

    // 设置响应头，提⽰浏览器下载⽂件
    QHttpServerResponse httpResp(fileData, QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/octet-stream");
    return httpResp;
}

// 获取弹幕数据
QHttpServerResponse HttpServer::getBarrage(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[getBarrage] 收到 getBarrage 请求, requestId = " <<jsonReq["requestId"].toString();

    // 构造时间不同弹幕
    QString videoId = jsonReq["videoId"].toString();
    QList<BarrageInfo> barrageList;
    for(int i = 0; i < 3; ++i)
    {
        int64_t playTime = i+2;
        BarrageInfo barrage("1000001", i+2, "cillllillli" + QString::number(i));
        barrageList.append(barrage);
    }

    // 构造相同时间点的弹幕数据
    for(int i = 0; i < 5; ++i)
    {
        int playTime = 3;
        BarrageInfo barrage("1000001", playTime, "billbiliilll" + QString::number(i));
        barrageList.append(barrage);
    }

    // 构造响应体
    QJsonArray barrageArray;
    for(int i = 0; i < barrageList.size(); ++i)
    {
        BarrageInfo &barrage = barrageList[i];
        QJsonObject barrageJson;
        barrageJson["barrageId"] = "50000"+QString::number(i);
        barrageJson["userId"] = barrage.userId;
        barrageJson["barrageTime"] = barrage.playTime;
        barrageJson["barrageContent"] = barrage.text;
        barrageArray.append(barrageJson);
    }
    QJsonObject jsonResp;
    QJsonObject jsonBody;
    jsonBody["barrageList"] = barrageArray;

    jsonResp["data"] = jsonBody;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

QHttpServerResponse HttpServer::setPlay(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[setPlay] 收到 setPlay 请求, requestId = " <<jsonReq["requestId"].toString();


    QString videoId = jsonReq["videoId"].toString();

    QJsonObject jsonResp;

    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

QHttpServerResponse HttpServer::judgeLike(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[judgeLike] 收到 judgeLike 请求, requestId = " <<jsonReq["requestId"].toString();


    QJsonObject jsonResp;
    QJsonObject jsonBody;

    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    jsonBody["isLike"] = true;
    jsonResp["data"] = jsonBody;

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

QHttpServerResponse HttpServer::setLike(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[setLike] 收到 setLike 请求, requestId = " <<jsonReq["requestId"].toString();


    QString videoId = jsonReq["videoId"].toString();

    QJsonObject jsonResp;

    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}

QHttpServerResponse HttpServer::newBarrage(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject &jsonReq = docReq.object();

    LOG() << "[newBarrage] 收到 newBarrage 请求, requestId = " <<jsonReq["requestId"].toString();

    QString videoId = jsonReq["videoId"].toString();

    // 解析出弹幕信息
    BarrageInfo barrageInfo;
    QJsonObject barrageObj = jsonReq["barrageInfo"].toObject();
    barrageInfo.playTime = barrageObj["barrageTime"].toInteger();
    barrageInfo.text = barrageObj["barrageContent"].toString();
    LOG() << "视频" << videoId << "弹幕" << barrageInfo.playTime <<barrageInfo.text;

    QJsonObject jsonResp;

    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    //  返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
    return httpResp;
}
