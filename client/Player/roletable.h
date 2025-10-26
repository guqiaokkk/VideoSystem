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
    // 将RoleTableItem添加到表格
    void updateRoleTable();

    // edituserdialog按钮点击触发
    void onInsertBtnClicked();

private:
    // 初始化样式表
    void initStyleSheet();

private:
    Ui::RoleTable *ui;

    // 保存按钮样式
    QMap<QString, QString> styleSheet;

    // 分⻚器指针
    Paginator* paginator = nullptr;
};

#endif // ROLETABLE_H
