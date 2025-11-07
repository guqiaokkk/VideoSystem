#ifndef CHECKTABLE_H
#define CHECKTABLE_H

#include "paginator.h"

#include <QWidget>

namespace Ui {
class CheckTable;
}

class CheckTable : public QWidget
{
    Q_OBJECT

public:
    explicit CheckTable(QWidget *parent = nullptr);
    ~CheckTable();

private:
    // 更新审核⻚⾯table
    void updateCheckTable(const QString &userId, const QString whichPage);

    // 重置按钮点击槽函数
    void onResetBtnClicked();

    // 查询按钮点击槽函数
    void onQueryBtnClicked();

    // 获取视频列表
    void getVideoList(int page);

    // 重置视频列表
    void resetPaginator(int pageCount);

private:
    Ui::CheckTable *ui;

    // 分⻚器指针
    Paginator *paginator = nullptr;

    // 当前获取的是第⼏⻚
    int page = 1;
};

#endif // CHECKTABLE_H
