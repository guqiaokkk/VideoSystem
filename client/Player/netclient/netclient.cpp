#include "netclient.h"

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <qjsonarray.h>

#include "util.h"
#include "../model/datacenter.h"


namespace network{

NetClient::NetClient(model::DataCenter *dataCenter)
    :dataCenter(dataCenter)
{
    // 获取服务器地址
    HTTP_URL = dataCenter->getServerURL();
}

void network::NetClient::hello()
{
    // 1. 构造请求体 body
    QJsonObject reqBody;

    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/hello", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        // a. 解析HTTP响应
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);

        // b. 判定响应是否出错
        if(!ok){
            LOG()<<"hello 请求出错，reason = "<<reason;
            return;
        }

        // c. 解析响应体中，服务端交给客户端的具体数据
        QJsonObject resoBody = respObj["data"].toObject();
        LOG() << resoBody["hello"].toString();

        // d . 数据保存
        // dataCenter->saveData(...);

        // e.通过界面做一些数据的展示处理   (根据发送的请求来确定是否需要)
        emit dataCenter->helloDone();
    });
}

void NetClient::ping()
{
    // 1. 构造请求体 body
    QJsonObject reqBody;

    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/ping", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        if(!ok){
            LOG()<<"hello 请求出错，reason = "<<reason;
            return;
        }
        QJsonObject resoBody = respObj["data"].toObject();
        LOG() << resoBody["ping"].toString();
    });
}

void NetClient::tempLogin()
{
    // 1. 构造请求
    QJsonObject reqBody;

    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/tempLogin", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        if(!ok){
            LOG()<<"tempLogin 请求出错，reason = "<<reason;
            return;
        }

        //  解析sessionId
        QJsonObject sessionJson = respObj["data"].toObject();
        dataCenter->setSeesionId(sessionJson["sessionId"].toString());
        LOG()<<"tempLogin 登录成功, requestId = "<<respObj["requestId"].toString();
        emit dataCenter->loginTempUserDone();
    });

}

void NetClient::getAllVideoList()
{
    // 1. 构造请求
    auto videoListPtr = dataCenter->getVideoListPtr();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["pageIndex"] = videoListPtr->getPageIndex();
    reqBody["pageCount"] = model::VideoList::PAGE_COUNT;
    // ⻚索引++，下⼀次能获取到正确⻚
    videoListPtr->setPageIndex(videoListPtr->getPageIndex() + 1);

    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/allVideoList", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        if(!ok){
            LOG()<<"allVideoList 请求出错，reason = "<<reason;
            return;
        }

        //  解析响应体中服务端交给客户端的具体数据
        QJsonObject resultObj = respObj["data"].toObject();
        // 将获取到的视频信息保存到视频列表
        dataCenter->setVideoList(resultObj);

        // 发射信号，通知界⾯更视频显⽰
        emit dataCenter->getAllVideoListDone();

        LOG()<<"allVideoList 成功, resquestId = "<<respObj["requestId"].toString();
    });
}

void NetClient::getAllVideosInKind(int kindId)
{
    // 1. 构造请求
    auto videoListPtr = dataCenter->getVideoListPtr();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["typeVideoList"] = kindId;
    reqBody["pageIndex"] = videoListPtr->getPageIndex();
    reqBody["pageCount"] = model::VideoList::PAGE_COUNT;
    // ⻚索引++，下⼀次能获取到正确⻚
    videoListPtr->setPageIndex(videoListPtr->getPageIndex() + 1);

    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/typeVideoList", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        if(!ok){
            LOG()<<"typeVideoList 请求出错，reason = "<<reason;
            return;
        }

        //  解析响应体中服务端交给客户端的具体数据
        QJsonObject resultObj = respObj["data"].toObject();
        // 将获取到的视频信息保存到视频列表
        dataCenter->setVideoList(resultObj);

        // 发射信号，通知界⾯更视频显⽰
        emit dataCenter->getAllVideoInKindDone();

        LOG()<<"typeVideoList 成功, resquestId = "<<respObj["requestId"].toString();
    });
}

void NetClient::getAllVideosInTag(int tagId)
{
    // 1. 构造请求
    auto videoListPtr = dataCenter->getVideoListPtr();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["videoTag"] = tagId;
    reqBody["pageIndex"] = videoListPtr->getPageIndex();
    reqBody["pageCount"] = model::VideoList::PAGE_COUNT;
    // ⻚索引++，下⼀次能获取到正确⻚
    videoListPtr->setPageIndex(videoListPtr->getPageIndex() + 1);

    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/tagVideoList", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        if(!ok){
            LOG()<<"tagVideoList 请求出错，reason = "<<reason;
            return;
        }

        //  解析响应体中服务端交给客户端的具体数据
        QJsonObject resultObj = respObj["data"].toObject();
        // 将获取到的视频信息保存到视频列表
        dataCenter->setVideoList(resultObj);

        // 发射信号，通知界⾯更视频显⽰
        emit dataCenter->getAllVideoInTagDone();

        LOG()<<"tagVideoList 成功, resquestId = "<<respObj["requestId"].toString();
    });
}

void NetClient::getVideosBySearchText(const QString &searchText)
{
    // 1. 构造请求
    auto videoListPtr = dataCenter->getVideoListPtr();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["searchKey"] = searchText;
    reqBody["pageIndex"] = videoListPtr->getPageIndex();
    reqBody["pageCount"] = model::VideoList::PAGE_COUNT;
    // ⻚索引++，下⼀次能获取到正确⻚
    videoListPtr->setPageIndex(videoListPtr->getPageIndex() + 1);

    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/keyVideoList", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        if(!ok){
            LOG()<<"keyVideoList 请求出错，reason = "<<reason;
            return;
        }

        //  解析响应体中服务端交给客户端的具体数据
        QJsonObject resultObj = respObj["data"].toObject();
        // 将获取到的视频信息保存到视频列表
        dataCenter->setVideoList(resultObj);

        // 发射信号，通知界⾯更视频显⽰
        emit dataCenter->getAllVideoInTagDone();

        LOG()<<"keyVideoList 成功, resquestId = "<<respObj["requestId"].toString();
    });
}

void NetClient::downloadPhoto(const QString &photoFileId)
{
    // 1. 构造请求
    QString queryString;
    queryString += "requestId=";
    queryString += makeRequestId();
    queryString += "&";
    queryString += "sessionId=";
    queryString += dataCenter->getLogingSessionId();
    queryString += "&";
    queryString += "fileId=";
    queryString += photoFileId;

    // 2. 发送请求
    QNetworkRequest httpReq;
    httpReq.setUrl(QUrl(HTTP_URL + "/HttpService/downloadPhoto?" + queryString));
    QNetworkReply *httpReply = httpClient.get(httpReq);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        // 解析响应
        // 1. 判定 HTTP 层⾯上, 是否出错
        if (httpReply->error() != QNetworkReply::NoError)
        {
            LOG() << httpReply->errorString();
            httpReply->deleteLater();
            return;
        }

        // 2. 获取图⽚数据
        // 发射信号，通知界⾯更视频显⽰
        QByteArray imgData = httpReply->readAll();

        emit dataCenter->downloadPhotoDone(photoFileId, imgData);
        httpReply->deleteLater();

        //LOG() << "downloadPhoto请求结束，图⽚下载成功";
    });
}

void NetClient::downloadVideo(const QString &videoFileId)
{
    // 1. 构造请求
    QString queryString;
    queryString += "fileId=";
    queryString += videoFileId;

    // 2. 发送请求
    QNetworkRequest httpReq;
    httpReq.setUrl(QUrl(HTTP_URL + "/HttpService/downloadVideo?" + queryString));
    QNetworkReply *httpReply = httpClient.get(httpReq);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        // 解析响应
        // 1. 判定 HTTP 层⾯上, 是否出错
        if (httpReply->error() != QNetworkReply::NoError)
        {
            LOG() << httpReply->errorString();
            httpReply->deleteLater();
            return;
        }

        // 2. 获取图⽚数据
        // 从服务器上拿到的是⼀个m3u8⽂件，该⽂件保存到磁盘中，然后将⽂件路径交给mpv播放
        QByteArray videoFileContent = httpReply->readAll();

        QString videoFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        videoFilePath += "/";
        videoFilePath += videoFileId;
        videoFilePath += ".m3u8";
        LOG()<<videoFilePath;
        writeByteArrayToFile(videoFilePath, videoFileContent);

        emit dataCenter->downloadVideoDone(videoFilePath, videoFileId);

        LOG() << "downloadVideo请求结束，视频下载成功";
    });
}

void NetClient::getVideoBarrage(const QString &videoId)
{
    // 1. 构造请求
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["videoId"] = videoId;


    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/getBarrage", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=]()mutable{
        // 1. 解析响应
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        // 2. 判定响应是否出错
        if(!ok){
            LOG()<<"keyVideoList 请求出错，reason = "<<reason;
            return;
        }

        auto dataCenter = model::DataCenter::getInstance();
        //  解析响应体中服务端交给客户端的具体数据
        QJsonObject resultObj = respObj["data"].toObject();
        // 将获取到的视频信息保存到视频列表
        dataCenter->setBarragesData(resultObj["barrageList"].toArray());

        // 发射信号，通知界⾯更视频显⽰
        emit dataCenter->getVideoBarrageDone(videoId);

        LOG() << "getBarrage请求结束，弹幕获取成功, requestId: " << respObj["requestId"].toString();;
    });
}

void NetClient::setPlayNumber(const QString &videoId)
{
    // 1. 构造请求
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["videoId"] = videoId;


    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/getBarrage", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=]()mutable{
        // 1. 解析响应
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        // 2. 判定响应是否出错
        if(!ok){
            LOG()<<"setPlay 请求出错，reason = "<<reason;
            return;
        }

        LOG() << "setPlay请求结束，弹幕获取成功, requestId: " << respObj["requestId"].toString();;
    });
}

void NetClient::getIsLikeVideo(const QString &videoId)
{
    // 1. 构造请求
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["videoId"] = videoId;


    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/judgeLike", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=]()mutable{
        // 1. 解析响应
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        // 2. 判定响应是否出错
        if(!ok){
            LOG()<<"judgeLike 请求出错，reason = "<<reason;
            return;
        }

        // 3. 发送信号通知界⾯更新
        //  解析响应体中服务端交给客户端的具体数据
        QJsonObject resultObj = respObj["data"].toObject();

        // 发射信号，通知界⾯更视频显⽰
        emit dataCenter->getIsLikeVideoDone(videoId, resultObj["isLike"].toBool());
        LOG() << "judgeLike 成功, requestId=" << respObj["requestId"].toString();

    });
}

void NetClient::setLikeNumber(const QString &videoId)
{
    // 1. 构造请求
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["videoId"] = videoId;


    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/setLike", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=]()mutable{
        // 1. 解析响应
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        // 2. 判定响应是否出错
        if(!ok){
            LOG()<<"setLike 请求出错，reason = "<<reason;
            return;
        }

        LOG() << "setLike请求结束，点赞成功, requestId: " << respObj["requestId"].toString();
    });
}

void NetClient::loadupBarrages(const QString &videoId, const model::BarrageInfo &barrageInfo)
{
    // 1. 构造请求
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLogingSessionId();
    reqBody["videoId"] = videoId;

    QJsonObject barrageObj;
    barrageObj["barrageContent"] = barrageInfo.text;
    barrageObj["barrageTime"] = barrageInfo.playTime;
    reqBody["barrageInfo"] = barrageObj;

    // 2. 发送请求
    QNetworkReply *httpReply = sendHttpRequest("/HttpService/newBarrage", reqBody);

    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=]()mutable{
        // 1. 解析响应
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpReply, ok, reason);
        // 2. 判定响应是否出错
        if(!ok){
            LOG()<<"newBarrage 请求出错，reason = "<<reason;
            return;
        }

        LOG() << "newBarrage请求结束，弹幕上传成功, requestId: " << respObj["requestId"].toString();
    });
}

QString NetClient::makeRequestId()
{
    return "R" + QUuid::createUuid().toString().sliced(25, 12);
}

// 通过这个函数, 把发送 HTTP 请求操作封装
QNetworkReply *NetClient::sendHttpRequest(const QString &resourcePath, QJsonObject &jsonBody)
{
    // 设置请求id
    jsonBody["requestId"] = makeRequestId();

    // 2. 发送请求
    // 设置 HTTP 请求
    QNetworkRequest httpReq;
    httpReq.setUrl(QUrl(HTTP_URL + resourcePath));
    httpReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf8");

    // 序列化 JSON
    QJsonDocument document(jsonBody);

    // post
    QNetworkReply *httpResp = httpClient.post(httpReq, document.toJson());

    return httpResp;
}

QJsonObject NetClient::handleHttpResponse(QNetworkReply *httpResp, bool &ok, QString &reason)
{
    //得到服务器传来的响应了

    // 判定Http层⾯是否出错
    if(httpResp->error() != QNetworkReply::NoError)
    {
        ok = false;
        reason = httpResp->errorString();
        httpResp->deleteLater();
        return QJsonObject();
    }

    // 获取到响应的body
    QByteArray respBody = httpResp->readAll();

    // 针对body反序列化
    QJsonDocument jsonDoc = QJsonDocument::fromJson(respBody);
    if(jsonDoc.isNull())
    {
        ok = false;
        reason = "解析 JSON ⽂件失败! JSON ⽂件格式有错误!";
        httpResp->deleteLater();
        return QJsonObject();
    }

    // 判定业务上的逻辑是否正确
    QJsonObject respObj = jsonDoc.object();
    if(respObj["errorCode"].toInt() != 0)       // 错误码0表⽰没有错误
    {
        ok = false;
        reason = respObj["errorMsg"].toString();
        httpResp->deleteLater();
        return respObj;
    }

    // 5. 释放 httpResp 对象
    httpResp->deleteLater();
    ok = true;
    return respObj;
}

}
