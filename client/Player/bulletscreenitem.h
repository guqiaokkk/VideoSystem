#ifndef BULLETSCREENITEM_H
#define BULLETSCREENITEM_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPropertyAnimation>

class BulletScreenInfo
{
public:
    QString userId;     // 发送弹幕⽤⼾
    QString videoId;    // 弹幕对应的视频Id
    int64_t playTime;   // 发送弹幕时当前播放时间
    QString text;       // 弹幕内容

    BulletScreenInfo(const QString &userId = "", int64_t playTime = 0, const QString &text = "")
        :userId(userId)
        ,playTime(playTime)
        ,text(text)
    {}
};



class BulletScreenItem : public QFrame
{
    Q_OBJECT
public:
    explicit BulletScreenItem(QWidget *parent = nullptr);

    // 设置弹幕中⽂本
    void setBulletScreenText(const QString &content);
    // 设置弹幕⽤⼾头像
    void setBulletScreenIcon(QPixmap &pixmap);

    // 给弹幕添加动画
    void setBulletScreenAnimation(int x, int duration);
    // 开启动画
    void startAnimation();


private:
    QLabel *text;       // 弹幕中⽂本
    QLabel *imgLabel;   // 弹幕中⽤⼾头像

    QPropertyAnimation *animation; // 动画对象实例指针
};

#endif // BULLETSCREENITEM_H
