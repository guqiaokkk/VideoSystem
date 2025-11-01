#include "playerserver.h"
#include "httpserver.h"
#include "util.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 启动Http服务器
    HttpServer *httpServer = HttpServer::getInstance();
    if(!httpServer->init())
    {
        LOG()<<"HTTP服务器启动失败!";
    }
    LOG()<<"HTTP服务器启动成功!";

    PlayerServer w;
    w.show();
    return a.exec();
}
