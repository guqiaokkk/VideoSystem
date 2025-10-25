#ifndef VIDEOBOX_H
#define VIDEOBOX_H

#include "playerpage.h"

#include <QWidget>

namespace Ui {
class VideoBox;
}

class VideoBox : public QWidget
{
    Q_OBJECT

public:
    explicit VideoBox(QWidget *parent = nullptr);
    ~VideoBox();

    // 事件过滤
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void onPlayClicked();

private:
    Ui::VideoBox *ui;

    PlayerPage* playPage; // 播放⻚⾯指针
};

#endif // VIDEOBOX_H
