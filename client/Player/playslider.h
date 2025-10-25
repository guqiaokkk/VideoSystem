#ifndef PLAYSLIDER_H
#define PLAYSLIDER_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class PlaySlider;
}

class PlaySlider : public QWidget
{
    Q_OBJECT

public:
    explicit PlaySlider(QWidget *parent = nullptr);
    ~PlaySlider();


    // ⿏标按下、移动、释放事件
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);


private:
    // 改变outLine的geometry
    void moveSlider();

private:
    Ui::PlaySlider *ui;

    // 记录当前播放⻓度
    int playGrogress;

};

#endif // PLAYSLIDER_H
