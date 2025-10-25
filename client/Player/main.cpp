#include "player.h"

#include "startuppage.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 禁⽌窗⼝按照分辨率百分⽐缩放，必须套放在程序第⼀句
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy( Qt::HighDpiScaleFactorRoundingPolicy::Floor);

    QApplication a(argc, argv);

    // 创建启动⻚，并以模态对话框⽅式显⽰
    startupPage startuppage;
    startuppage.startup();// 开启定时器
    startuppage.exec();

    // 显⽰主界⾯

    Player w;
    w.show();
    return a.exec();
}
