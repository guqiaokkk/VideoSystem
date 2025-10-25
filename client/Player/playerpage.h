#ifndef PLAYERPAGE_H
#define PLAYERPAGE_H

#include "volume.h"
#include "playspeed.h"

#include <QWidget>

namespace Ui {
class PlayerPage;
}

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(QWidget *parent = nullptr);
    ~PlayerPage();

    // ⾳量调节
    void onVolumeBtnClicked();

    // 移动窗⼝控件(音量和倍速)
    void moveWindows(const QPoint &point);

    // 倍速播放
    void onSpeedBtnClicked();

private:
    Ui::PlayerPage *ui;

    // ⾳量调节窗⼝指针
    Volume *volume;

    // 倍数播放窗⼝
    PlaySpeed *playSpeed;
};

#endif // PLAYERPAGE_H
