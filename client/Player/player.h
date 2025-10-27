#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Player;
}
QT_END_NAMESPACE

enum StackWidgetPage{
    HomePage = 0,
    MyselfPage = 1,
    AdminPage = 2,
    UploadPage = 3,
};

class Player : public QWidget
{
    Q_OBJECT

public:

    ~Player();

    // 获取实例
    static Player *getInstance();

    // 绑定信号槽函数
    void connectSignalAndSlot();

    // 点击bodyLeft按钮，切换bodyRight中对应⻚⾯
    void onSwitchStackedWidgetPage(int pageId);
    // 切换按钮点击后，重置按钮上⽂本和图⽚
    void resetSwitchBtnInfo(int pageId);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void initUI();

    // 将构造函数私有化
    Player(QWidget *parent = nullptr);


private:
    Ui::Player *ui;

    //在head上按下时，⿏标相对于窗⼝左上⻆的偏移
    QPoint dragpos;

    // 添加单例实例
    static Player *instance;
};
#endif // PLAYER_H
