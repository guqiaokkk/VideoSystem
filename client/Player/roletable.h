#ifndef ROLETABLE_H
#define ROLETABLE_H

#include <QWidget>

#include "paginator.h"


namespace Ui {
class RoleTable;
}

class RoleTable : public QWidget
{
    Q_OBJECT

public:
    explicit RoleTable(QWidget *parent = nullptr);
    ~RoleTable();

    // 重置按钮点击
    void onResetBtnClicked();

    // 查询按钮点击
    void onQueryBtnClicked();


private slots:
    // 更新管理员列表信息到界⾯
    void updateRoleTable();

    // edituserdialog按钮点击触发
    void onInsertBtnClicked();

    // 重置分⻚器
    void resetPaginator(int pageCount);

private:
    // 初始化样式表
    void initStyleSheet();

    // 获取管理员列表
    void getAdminList(int page);

private:
    Ui::RoleTable *ui;

    // 保存按钮样式
    QMap<QString, QString> styleSheet;

    // 分⻚器指针
    Paginator* paginator = nullptr;

    // 记录分⻚器⻚数
    int page = 1;
};

#endif // ROLETABLE_H
