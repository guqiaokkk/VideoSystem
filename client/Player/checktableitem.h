#ifndef CHECKTABLEITEM_H
#define CHECKTABLEITEM_H

#include "./model/datacenter.h"
#include "PlayerPage.h"

#include <QWidget>

namespace Ui {
class CheckTableItem;
}

class CheckTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit CheckTableItem(QWidget *parent, model::VideoInfo videoInfo);
    ~CheckTableItem();

private slots:
    // 视频按钮点击
    void onVideoBtnClicked();

    // 操作按钮点击
    void onOperatorBtnClicked();

    // 驳回按钮点击
    void onOperatorBtn2Clicked();


private:
    // 设置样式
    void initStyleSheet();

private:
    Ui::CheckTableItem *ui;

    // 保存视频信息
    model::VideoInfo videoInfo;

    // 保存样式表
    QMap<QString, QString> styleSheet;

    // 视频播放相关
    PlayerPage *playerPage = nullptr;
    QPixmap userAvatar;
};

#endif // CHECKTABLEITEM_H
