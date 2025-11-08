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

        // 获取⽤⼾视频列表中：⽤⼾头像id、视频封⾯id与具体图⽚资源路径的映射关系
        // 该模块下的所有id都是从50000开始的
        resourceId = 50000;
        for(int i = 0; i < 100; ++i)
        {
            idPathTable.insert(QString::number(resourceId++), "/images/touxiang3.png");
            idPathTable.insert(QString::number(resourceId++), "/images/videoImage3.png");
            idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
        }

        // 假设上传视频Id从60000开始
        idPathTable.insert(QString::number(60000), "/videos/");


        // 获取状态视频列表中：⽤⼾头像id、视频封⾯id与具体图⽚资源路径的映射关系
        // 该模块下的所有id都是从70000开始的
        resourceId = 70000;
        for(int i = 0; i < 100; ++i)
        {
            idPathTable.insert(QString::number(resourceId++), "/images/touxiang3.png");
            idPathTable.insert(QString::number(resourceId++), "/images/ktll.png");
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

        // 上传视频
        httpServer.route("/HttpService/uploadVideo", [=](const QHttpServerRequest& req) {
            return this->uploadVideo(req);
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

        // 获取⽤⼾信息
        httpServer.route("/HttpService/getUserInfo", [=](const QHttpServerRequest& req) {
            return this->getUserInfo(req);
        });

        // 上传图⽚
        httpServer.route("/HttpService/uploadPhoto", [=](const QHttpServerRequest& req) {
            return this->uploadPhoto(req);
        });

        // 修改头像
        httpServer.route("/HttpService/setAvatar", [=](const QHttpServerRequest& req) {
            return this->setAvatar(req);
        });

        // 获取指定⽤⼾视频列表
        httpServer.route("/HttpService/userVideoList", [=](const QHttpServerRequest& req) {
            return this->userVideoList(req);
        });

        // 删除视频
        httpServer.route("/HttpService/removeVideo", [=](const QHttpServerRequest& req) {
            return this->removeVideo(req);
        });

        // 关注⽤⼾
        httpServer.route("/HttpService/newAttention", [=](const QHttpServerRequest& req) {
            return this->newAttention(req);
        });
        // 取消关注⽤⼾
        httpServer.route("/HttpService/delAttention", [=](const QHttpServerRequest& req) {
            return this->delAttention(req);
        });

        // 获取验证码
        httpServer.route("/HttpService/getCode", [=](const QHttpServerRequest& req) {
            return this->getCode(req);
        });

        // 密码登录
        httpServer.route("/HttpService/passwdLogin", [=](const QHttpServerRequest& req) {
            return this->passwdLogin(req);
        });

        // 验证码登录
        httpServer.route("/HttpService/vcodeLogin", [=](const QHttpServerRequest& req) {
            return this->vcodeLogin(req);
        });

        // 会话登录
        httpServer.route("/HttpService/sessionLogin", [=](const QHttpServerRequest& req) {
            return this->loginSession(req);
        });

        // 退出登录
        httpServer.route("/HttpService/logout", [=](const QHttpServerRequest& req) {
            return this->logout(req);
        });

        // 修改密码
        httpServer.route("/HttpService/setPassword", [=](const QHttpServerRequest& req) {
            return this->setPassword(req);
        });

        // 修改昵称
        httpServer.route("/HttpService/setNickname", [=](const QHttpServerRequest& req) {
            return this->setNickname(req);
        });

        // 新增视频信息
        httpServer.route("/HttpService/newVideo", [=](const QHttpServerRequest& req) {
            return this->newVideo(req);
        });

        // 获取状态视频列表
        httpServer.route("/HttpService/statusVideoList", [=](const QHttpServerRequest& req) {
            return this->statusVideoList(req);
        });

        // 审核视频
        httpServer.route("/HttpService/checkVideo", [=](const QHttpServerRequest& req) {
            return this->checkVideo(req);
        });

        // 上架视频
        httpServer.route("/HttpService/saleVideo", [=](const QHttpServerRequest& req) {
            return this->saleVideo(req);
        });

        // 下架视频
        httpServer.route("/HttpService/haltVideo", [=](const QHttpServerRequest& req) {
            return this->haltVideo(req);
        });

        // 获取管理员信息-⼿机号
        httpServer.route("/HttpService/getAdminByEmail", [=](const QHttpServerRequest& req) {
            return this->getAdminByEmail(req);
        });

        // 获取管理员列表-状态
        httpServer.route("/HttpService/getAdminListByStatus", [=](const QHttpServerRequest& req) {
            return this->getAdminListByStatus(req);
        });

        // 新增管理员
        httpServer.route("/HttpService/newAdministrator", [=](const QHttpServerRequest& req) {
            return this->newAdministrator(req);
        });

        // 编辑管理员
        httpServer.route("/HttpService/setAdministrator", [=](const QHttpServerRequest& req) {
            return this->setAdministrator(req);
        });

        // 设置⽤⼾状态
        httpServer.route("/HttpService/setStatus", [=](const QHttpServerRequest& req) {
            return this->setStatus(req);
        });

        // 删除管理员
        httpServer.route("/HttpService/delAdministrator", [=](const QHttpServerRequest& req) {
            return this->delAdministrator(req);
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
        //LOG() << "[downloadPhoto] 收到 downloadPhoto 请求, requestId=" << requestId;

        // 构造图⽚路径
        QDir dir(QDir::currentPath());
        dir.cdUp();
        dir.cdUp();
        QString imgPath = dir.absolutePath();
        imgPath += idPathTable[fileId];
        //LOG()<<"图⽚ID："<<fileId<<"--"<<imgPath;

        // 读取图⽚数据
        QByteArray imgData = loadFileToByteArray(imgPath);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(imgData, QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/octet-stream");
        return httpResp;

    }

    QHttpServerResponse HttpServer::uploadPhoto(const QHttpServerRequest &req)
    {
        // 解析查询字符串
        QUrlQuery query(req.url());
        const QString &requestId = query.queryItemValue("requestId");
        const QString &sessionId = query.queryItemValue("sessionId");
        LOG() << "[uploadPhoto] 收到 uploadPhoto 请求, requestId=" << requestId << ", sessionId=" << sessionId;

        // 解析出图⽚数据
        const QByteArray &imgData = req.body();

        // 构造图⽚路径
        QDir dir(QDir::currentPath());
        dir.cdUp();
        dir.cdUp();
        QString imgPath = dir.absolutePath();
        imgPath += "/images/temp.png";

        writeByteArrayToFile(imgPath, imgData);
        idPathTable["1000"] = "/images/temp.png";

        // 构造正⽂
        QJsonObject jsonResp;
        QJsonObject jsonBody;
        jsonBody["fileId"] = "1000" ;

        jsonResp["data"] = jsonBody;
        jsonResp["requestId"] = requestId;
        jsonResp["errorCode"] = 0;
        jsonResp["errorMsg"] = "";

        QJsonDocument docResp(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
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

    QHttpServerResponse HttpServer::uploadVideo(const QHttpServerRequest &req)
    {
        // 解析查询字符串
        QUrlQuery query(req.url());
        const QString &requestId = query.queryItemValue("requestId");
        const QString &sessionId = query.queryItemValue("sessionId");
        LOG() << "[uploadVideo] 收到 uploadVideo 请求, requestId=" << requestId << ", sessionId=" << sessionId;

        // 获取请求数据
        const QByteArray &videoData = req.body();

        // 构造图⽚路径 , 保存视频⽂件到videos⽬录下
        QDir dir(QDir::currentPath());
        dir.cdUp();
        dir.cdUp();
        QString videoPath = dir.absolutePath();
        videoPath += "/videos/222.mp4";

        writeByteArrayToFile(videoPath, videoData);

        // 构造正⽂
        QJsonObject jsonResp;
        QJsonObject jsonBody;
        jsonBody["fileId"] = "60000" ;

        jsonResp["data"] = jsonBody;
        jsonResp["requestId"] = requestId;
        jsonResp["errorCode"] = 0;
        jsonResp["errorMsg"] = "";

        QJsonDocument docResp(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
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

    QHttpServerResponse HttpServer::getUserInfo(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();
        const QString &userId = jsonReq["userId"].toString();

        LOG() << "[getUserInfo] 收到 getUserInfo 请求, requestId=" << jsonReq["requestId"].toString() << ", userId=" << userId;

        QJsonObject jsonResp;

        jsonResp["requestId"] = jsonReq["requestId"].toString();
        jsonResp["errorCode"] = 0;
        jsonResp["errorMsg"] = "";

        // 构造用户信息
        QJsonObject userInfoJson;
        if(userId.isEmpty()){
            // 当前用户个人信息
            userInfoJson["userId"] = "100001";             // 用户id
            userInfoJson["email"] = "15012345678";  // 用户邮箱
            userInfoJson["nickname"] = "张三";             // 用户昵称
            // 角色类型：0-未知用户  1-超级管理员  2-普通管理员  3-普通用户  4-临时用户
            QJsonArray roleTyleArray;
            roleTyleArray.append(3);
            userInfoJson["roleType"] = roleTyleArray;     // 用户角色
            // 身份类型: 0-位置  1-C端用户   2-B端用户
            QJsonArray identityTypeArray;
            identityTypeArray.append(2);
            userInfoJson["identityType"] = identityTypeArray;     // 用户身份
            userInfoJson["likeCount"] = 12345;             // 点赞数
            userInfoJson["playCount"] = 12346;             // 播放数
            userInfoJson["followedCount"] = 123;           // 关注数
            userInfoJson["followerCount"] = 123;           // 粉丝数
            // 用户状态：0-无状态  1-启用  2-禁用---用户状态主要是给管理员页面使用
            userInfoJson["userStatus"] = 0;               // 管理员状态
            userInfoJson["isFollowing"] = 0;              // 用户是否被关注
            userInfoJson["userMemo"] = "";                // 备注信息
            userInfoJson["userCTime"] = "";               // 用户创建时间
            userInfoJson["avatarFileId"] = "10000";       // 用户头像id
        }
        else
        {
            // 其他用户个人信息
            userInfoJson["userId"] = "100002";             // 用户id
            userInfoJson["email"] = "15012345678";  // 用户邮箱
            userInfoJson["nickname"] = "李四";             // 用户昵称
            // 角色类型：0-未知用户  1-超级管理员  2-普通管理员  3-普通用户  4-临时用户
            QJsonArray roleTyleArray;
            roleTyleArray.append(3);
            userInfoJson["roleType"] = roleTyleArray;     // 用户角色
            // 身份类型: 0-位置  1-C端用户   2-B端用户
            QJsonArray identityTypeArray;
            identityTypeArray.append(1);
            userInfoJson["identityType"] = identityTypeArray;     // 用户身份
            userInfoJson["likeCount"] = 23456;             // 点赞数
            userInfoJson["playCount"] = 23457;             // 播放数
            userInfoJson["followedCount"] = 234;           // 关注数
            userInfoJson["followerCount"] = 234;           // 粉丝数
            // 用户状态：0-无状态  1-启用  2-禁用---用户状态主要是给管理员页面使用
            userInfoJson["userStatus"] = 0;               // 管理员状态
            userInfoJson["isFollowing"] = 1;              // 用户是否被关注
            userInfoJson["userMemo"] = "";                // 备注信息
            userInfoJson["userCTime"] = "";               // 用户创建时间
            userInfoJson["avatarFileId"] = "20000";       // 用户头像id
        }

        QJsonObject resultJson;
        resultJson["userInfo"] = userInfoJson;
        jsonResp["data"] = resultJson;
        LOG()<<jsonResp;


        //  返回响应
        QJsonDocument docResp;
        docResp.setObject(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
        return httpResp;
    }

    QHttpServerResponse HttpServer::setAvatar(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[setAvatar] 收到 setAvatar 请求, requestId=" << jsonReq["requestId"].toString() << ", fileId=" << jsonReq["fileId"].toString();

        // 构造响应数据
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

    QHttpServerResponse HttpServer::userVideoList(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[userVideoList] 收到 userVideoList 请求, requestId=" << jsonReq["requestId"].toString();

        // 构造视频列表数据
        QJsonObject videoListObj;
        int pageCount = jsonReq["pageCount"].toInt();

        int videoId = 50000; int userId = 50000; int resourceId = 50000;

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
            videoJsonObj["videoStatus"] = rand()%4 + 1;
            videoJsonObj["checkerId"] = "1234";
            videoJsonObj["checkerName"] = "古桥";
            videoJsonObj["checkerAvatar"] = "";

            videoLists.append(videoJsonObj);
        }

        QJsonObject jsonBody;
        jsonBody["videoList"] = videoLists;
        jsonBody["totalCount"] = 100;


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

    QHttpServerResponse HttpServer::removeVideo(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[removeVideo] 收到 removeVideo 请求, requestId=" << jsonReq["requestId"].toString() << ", videoId=" << jsonReq["videoId"].toString();

        // 构造响应数据
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

    QHttpServerResponse HttpServer::newAttention(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[newAttention] 收到添加关注请求, requestId=" << jsonReq["requestId"].toString() << ", userId=" << jsonReq["userId"].toString();

        // 构造响应数据
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

    QHttpServerResponse HttpServer::delAttention(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[delAttention] 收到取消关注请求, requestId=" << jsonReq["requestId"].toString() << ", userId=" << jsonReq["userId"].toString();

        // 构造响应数据
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

    QHttpServerResponse HttpServer::getCode(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[getCode] 收到 getCode 请求, requestId=" << jsonReq["requestId"].toString();

        // 验证账号密码是否正确
        QString email = jsonReq["email"].toString();
        LOG()<<"给邮箱 "<<email<<" 发送验证码123456";

        // 构造响应数据
        QJsonObject jsonResp;
        jsonResp["requestId"] = jsonReq["requestId"].toString();
        jsonResp["errorCode"] = 0;
        jsonResp["errorMsg"] = "";

        QJsonObject jsonBody;
        jsonBody["codeId"] = "111111";

        jsonResp["data"] = jsonBody;

        //  返回响应
        QJsonDocument docResp;
        docResp.setObject(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
        return httpResp;
    }

    QHttpServerResponse HttpServer::vcodeLogin(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[vcodeLogin] 收到 vcodeLogin 请求, requestId=" << jsonReq["requestId"].toString();

        // 验证是否正确
        QString email = jsonReq["email"].toString();
        QString authcode = jsonReq["verifyCode"].toString();
        QString authcodeId = jsonReq["codeId"].toString();

        LOG()<<"短信登录email: "<<email<<" 验证码: "<<authcode<<" 验证码id: " << authcodeId;

        int errorCode = 0;
        QString errorMsg;

        if(email != "15012345678")
        {
            errorCode = 600;
            errorMsg = "⼿机号输⼊有误";
        }
        if(authcode != "123456")
        {
            errorCode = 601;
            errorMsg = "验证码输⼊有误";
        }
        if(authcodeId != "111111")
        {
            errorCode = 602;
            errorMsg = "验证码id输⼊有误";
        }


        // 构造响应数据
        QJsonObject jsonResp;
        jsonResp["requestId"] = jsonReq["requestId"].toString();
        jsonResp["errorCode"] = errorCode;
        jsonResp["errorMsg"] = errorMsg;

        QJsonObject jsonBody;
        jsonBody["codeId"] = "111111";

        jsonResp["data"] = jsonBody;

        //  返回响应
        QJsonDocument docResp;
        docResp.setObject(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
        return httpResp;
    }

    QHttpServerResponse HttpServer::passwdLogin(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[passwdLogin] 收到 passwdLogin 请求, requestId=" << jsonReq["requestId"].toString();

        // 验证是否正确
        QString email = jsonReq["email"].toString();
        QString password = jsonReq["password"].toString();

        LOG()<<"短信登录email: " <<email << " 登录密码："<<password;

        int errorCode = 0;
        QString errorMsg;

        if(email != "15012345678")
        {
            errorCode = 603;
            errorMsg = "⼿机号输⼊有误";
        }
        if(password != "hello123")
        {
            errorCode = 604;
            errorMsg = "密码错误!!!";
        }

        // 构造响应数据
        QJsonObject jsonResp;
        jsonResp["requestId"] = jsonReq["requestId"].toString();
        jsonResp["errorCode"] = errorCode;
        jsonResp["errorMsg"] = errorMsg;

        //  返回响应
        QJsonDocument docResp;
        docResp.setObject(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
        return httpResp;
    }

    QHttpServerResponse HttpServer::loginSession(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[sessionLogin] 收到 sessionLogin 请求, requestId=" << jsonReq["requestId"].toString();

        // 构造响应数据
        QJsonObject jsonResp;
        jsonResp["requestId"] = jsonReq["requestId"].toString();
        jsonResp["errorCode"] = 0;
        jsonResp["errorMsg"] = "";

        QJsonObject jsonBody;
        jsonBody["isGuest"] = false;
        jsonResp["data"] = jsonBody;

        //  返回响应
        QJsonDocument docResp;
        docResp.setObject(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
        return httpResp;
    }

    QHttpServerResponse HttpServer::logout(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[logout] 收到 logout 请求, requestId=" << jsonReq["requestId"].toString();

        // 将⽤⼾切换到临时⽤⼾，返回新的sessionId
        roleType = TempUser;

        // 构造响应数据
        QJsonObject jsonResp;
        jsonResp["requestId"] = jsonReq["requestId"].toString();
        jsonResp["errorCode"] = 0;
        jsonResp["errorMsg"] = "";

        QJsonObject jsonBody;
        jsonBody["sessionId"] = jsonReq["sessionId"].toString();
        jsonResp["data"] = jsonBody;

        //  返回响应
        QJsonDocument docResp;
        docResp.setObject(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
        return httpResp;
    }

    QHttpServerResponse HttpServer::setPassword(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[setPassword] 收到 setPassword 请求, requestId=" << jsonReq["requestId"].toString();

        // 构造响应数据
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

    QHttpServerResponse HttpServer::setNickname(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[setNickname] 收到 setNickname 请求, requestId=" << jsonReq["requestId"].toString() << ", nickname=" << jsonReq["nickname"].toString();

        // 构造响应数据
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

    QHttpServerResponse HttpServer::newVideo(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[newVideo] 收到 newVideo 请求, requestId=" << jsonReq["requestId"].toString();
        // 解析上传视频信息
        QJsonObject videoInfo = jsonReq["data"].toObject();

        QString videoFileId = videoInfo["videoFileId"].toString();
        QString photoFileId = videoInfo["photoFileId"].toString();
        QString videoTitle = videoInfo["videoTitle"].toString();
        int videoType = videoInfo["videoType"].toInteger();
        QString videoDesc = videoInfo["videoDesc"].toString();
        int64_t duration = videoInfo["duration"].toInteger();

        LOG()<<"videoFileId : " << videoFileId;
        LOG()<<"photoFileId : " << photoFileId;
        LOG()<<"videoTitle : " << videoTitle;
        LOG()<<"videoDesc : " << videoDesc;
        LOG()<<"videoType : " << videoType;
        LOG()<<"duration : " << duration;

        QJsonArray tagIds = videoInfo["videoTag"].toArray();
        for(int i = 0; i < tagIds.size(); ++i)
        {
            int tagId = tagIds[i].toInt();
            LOG()<< "tagId" << i <<"-"<< tagId;
        }

        // 构造响应数据
        QJsonObject jsonResp;
        jsonResp["requestId"] = jsonReq["requestId"].toString();
        jsonResp["errorCode"] = 0;
        jsonResp["errorMsg"] = "";

        //  返回响应
        QJsonDocument docResp;
        docResp.setObject(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
        return httpResp;
    }

    QHttpServerResponse HttpServer::statusVideoList(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[statusVideoList] 收到 statusVideoList 请求, requestId=" << jsonReq["requestId"].toString();

        // 构造视频列表数据
        int pageCount = jsonReq["pageCount"].toInteger();
        int videoId = 70000; int userId = 70000; int resourceId = 70000;
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

            int videoStatue = jsonReq["videoStatus"].toInteger();
            if(videoStatue == 0)
            {
                videoJsonObj["videoStatus"] = rand()%4 + 1;
            }
            else
            {
                videoJsonObj["videoStatus"] = videoStatue;
            }

            videoJsonObj["checkerId"] = "1234";
            videoJsonObj["checkerName"] = "古桥kkkkk";
            videoJsonObj["checkerAvatar"] = "";

            videoLists.append(videoJsonObj);
        }

        QJsonObject jsonBody;
        jsonBody["videoList"] = videoLists;
        jsonBody["totalCount"] = 300;


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

    QHttpServerResponse HttpServer::checkVideo(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[checkVideo] 收到 checkVideo 请求, requestId=" << jsonReq["requestId"].toString();

        bool checkResult = jsonReq["checkResult"].toBool();
        QString videoId = jsonReq["videoId"].toString();

        if(checkResult)
        {
            LOG()<<"视频 "<< videoId <<"审核通过";
        }
        else
        {
            LOG()<<"视频 "<< videoId <<"审核驳回";
        }

        // 构造响应体
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

    QHttpServerResponse HttpServer::saleVideo(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[saleVideo] 收到 saleVideo 请求, requestId=" << jsonReq["requestId"].toString();

        QString videoId = jsonReq["videoId"].toString();

        LOG()<<"视频 "<< videoId <<"上架";

        // 构造响应体
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

    QHttpServerResponse HttpServer::haltVideo(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[haltVideo] 收到 haltVideo 请求, requestId=" << jsonReq["requestId"].toString();

        QString videoId = jsonReq["videoId"].toString();

        LOG()<<"视频 "<< videoId <<"下架";

        // 构造响应体
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

    QHttpServerResponse HttpServer::getAdminByEmail(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[getAdminByEmail] 收到 getAdminByEmail 请求, requestId=" << jsonReq["requestId"].toString();

        QString email = jsonReq["email"].toString();
        LOG()<<"email "<< email;

        int userId = 1234;
        QJsonObject userInfoJson;
        userInfoJson["userId"] = QString::number(userId);
        userInfoJson["nickname"] = "平台管理员";
        userInfoJson["roleType"] = 2; // 平台管理员
        userInfoJson["email"] = email;
        userInfoJson["userStatu"] = rand()%2 + 1; // 状态为1-启⽤ 2-禁⽌
        userInfoJson["userMemo"] = "有审核视频权限";

        QJsonObject jsonBody;
        jsonBody["userInfo"] = userInfoJson;
        // 构造响应体
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

    QHttpServerResponse HttpServer::getAdminListByStatus(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[getAdminListByStatus] 收到 getAdminListByStatus 请求, requestId=" << jsonReq["requestId"].toString();

        int adminStatus = jsonReq["userStatus"].toInt();
        if(0 == adminStatus){
            LOG()<<"获取所有管理员信息列表";
        }else if(1 == adminStatus){
            LOG()<<"获取启⽤管理员信息列表";
        }else{
            LOG()<<"获取禁⽌管理员信息列表";
        }

        QJsonObject jsonBody;
        jsonBody["totalCount"] = 200;

        // 构造⼀⻚的管理员信息
        QJsonArray adminListJson;
        int pageCount = jsonReq["pageCount"].toInt();
        QStringList remarks = {"视频审核", "运营⼈员2", "aaaaaa", "tiger"};
        QStringList nickNames = {"管理员", "李信", "guqiao", "文乃"};
        int userId = 1234;
        long long email = 15012345678;
        for(int i = 0; i < pageCount; ++i)
        {
            QJsonObject adminInfoJson;
            adminInfoJson["userId"] = QString::number(userId++);
            adminInfoJson["nickname"] = nickNames[rand()%4];
            adminInfoJson["roleType"] = 2; // 平台管理员
            adminInfoJson["email"] = QString::number(email++);

            if(adminStatus == 0)
            {
                // 获取所有状态为禁⽌ 或者 启⽤ 的管理员
                adminInfoJson["userStatu"] = rand()%2 + 1; // 状态为1-启⽤ 2-禁⽌
            }
            else
            {
                adminInfoJson["userStatu"] = adminStatus;
            }
            adminInfoJson["userMemo"] = remarks[rand()%4];

            adminListJson.append(adminInfoJson);
        }
        jsonBody["userList"] = adminListJson;

        // 构造响应体
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

    QHttpServerResponse HttpServer::newAdministrator(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[newAdministrator] 收到 newAdministrator 请求, requestId=" << jsonReq["requestId"].toString();

        QJsonObject adminJson = jsonReq["userInfo"].toObject();
        LOG()<<"新增管理员信息：";
        LOG()<<"nickName: "<<adminJson["nickname"].toString();

        int roleType = adminJson["roleType"].toInt();
        if(1 == roleType)
        {
            LOG()<<"roleType: 超级管理员";
        }
        else if(2 == roleType)
        {
            LOG()<<"roleType: 普通管理员";
        }

        int userStatue = adminJson["userStatu"].toInt();
        if(1 == userStatue)
        {
            LOG()<<"userStatu: 启⽤";
        }
        else if(2 == userStatue)
        {
            LOG()<<"userStatu: 禁⽌";
        }

        LOG()<<"userMemo: "<<adminJson["userMemo"].toString();
        LOG()<<"email: "<<adminJson["email"].toString();

        // 构造响应体
        QJsonObject jsonResp;
        jsonResp["requestId"] = jsonReq["requestId"].toString();
        jsonResp["errorCode"] = 0;
        jsonResp["errorMsg"] = "";

        QJsonObject jsonBody;
        jsonBody["userId"] = "1234";

        jsonResp["data"] = jsonBody;

        //  返回响应
        QJsonDocument docResp;
        docResp.setObject(jsonResp);

        // 构造 HTTP 响应
        QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
        httpResp.setHeader("Content-Type", "application/json; charset = utf-8");
        return httpResp;
    }

    QHttpServerResponse HttpServer::setAdministrator(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[setAdministrator] 收到 setAdministrator 请求, requestId=" << jsonReq["requestId"].toString();

        QJsonObject adminJson = jsonReq["userInfo"].toObject();
        LOG()<<"编辑管理员"<<adminJson["userId"].toString()<<"信息:";
        LOG()<<"nickName: "<<adminJson["nickname"].toString();

        int userStatus = adminJson["userStatus"].toInt();
        if(1 == userStatus)
        {
            LOG()<<"userStatus: 启⽤";
        }
        else if(2 == userStatus)
        {
            LOG()<<"userStatus: 禁⽌";
        }
        LOG()<<"userMemo: "<<adminJson["userMemo"].toString();

        // 构造响应体
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

    QHttpServerResponse HttpServer::setStatus(const QHttpServerRequest &req)
    {

        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[setStatus] 收到 setStatus 请求, requestId=" << jsonReq["requestId"].toString();

        int adminStatus = jsonReq["userStatus"].toInt();
        QString adminId = jsonReq["userId"].toString();
        QString status = "启⽤";
        if(adminStatus == 2)
        {
           status = "禁⽤";
        }
        LOG()<<"管理员: "<<adminId<<"状态修改为"<<status;

        // 构造响应体
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

    QHttpServerResponse HttpServer::delAdministrator(const QHttpServerRequest &req)
    {
        QJsonDocument docReq = QJsonDocument::fromJson(req.body());
        const QJsonObject &jsonReq = docReq.object();

        LOG() << "[delAdministrator] 收到 delAdministrator 请求, requestId=" << jsonReq["requestId"].toString();


        QString adminId = jsonReq["userId"].toString();
        LOG()<<"管理员 "<<adminId<<"删除成功";

        // 构造响应体
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

