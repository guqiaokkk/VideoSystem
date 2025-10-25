#ifndef AVATARBUTTON_H
#define AVATARBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class AvatarButton : public  QPushButton
{
    Q_OBJECT
public:
    explicit AvatarButton(QWidget *parent = nullptr);

    // 设置是否显⽰遮罩层，默认情况下显⽰
    void changeMode(bool showMask);

protected:
    // ⿏标进⼊控件事件
    void enterEvent(QEnterEvent *event);
    // ⿏标离开控件事件
    void leaveEvent(QEvent *event);

private:
    QLabel *mask;   // 遮罩层
    bool showMask;  // 是否显⽰遮罩层
};

#endif // AVATARBUTTON_H
