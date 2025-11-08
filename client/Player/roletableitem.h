#ifndef ROLETABLEITEM_H
#define ROLETABLEITEM_H

#include "./model/data.h"

#include <QWidget>

namespace Ui {
class RoleTableItem;
}

class RoleTableItem : public QWidget
{
    Q_OBJECT

public:
    explicit RoleTableItem(QWidget *parent, model::AdminInfo &adminInfo, int seqNumber = 1);
    ~RoleTableItem();

    // 将管理员信息更新到界⾯
    void updateUI(int seqNumber);

private slots:
    // 编辑按钮点击
    void onEditBtnClicked();

    // 状态按钮点击
    void onStatusButtonClicked();

    // 删除按钮点击
    void onDelBtnClicked();

private:
    // 初始化样式表
    void initStyleSheet();

    // 检测当前登录⽤⼾是否是管理员⾃⼰
    bool logUserisAdminMyself(const QString& text);


private:
    Ui::RoleTableItem *ui;

    // 样式表映射
    QMap<QString, QString> styleSheet;
    // 单个管理员信息
    model::AdminInfo adminInfo;
};

#endif // ROLETABLEITEM_H
