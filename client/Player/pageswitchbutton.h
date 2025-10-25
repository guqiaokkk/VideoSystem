#ifndef PAGESWITCHBUTTON_H
#define PAGESWITCHBUTTON_H

#include <QPushButton>
#include <QLabel>

class PageSwitchButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PageSwitchButton(QWidget *parent = nullptr);

    // 设置按钮上的⽂本和图⽚
    void setImageAndText(const QString &ImgPath, const QString &text, int pageId);

    // 重写QWidget中⿏标按下事件函数
    void mousePressEvent(QMouseEvent *event);

    //  设置按钮上图标
    void setImage(const QString &ImgPath);

    // 设置按钮上⽂本颜⾊
    void setTextColor(const QString &textColor = "#000000");

    // 获取当前按钮对应的⻚⾯id
    int getPageId()const;

signals:
    void switchPage(int pageId);    //⻚⾯切换信号

private:
    QLabel *btnImg;     // 按钮上图片
    QLabel *btnTittle;  // 按钮上⽂本
    int pageId;         // 标记按钮对应的⻚⾯在stackedWidget中的编号
};

#endif // PAGESWITCHBUTTON_H
