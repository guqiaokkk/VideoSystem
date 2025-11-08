#ifndef EDITUSERDIALOG_H
#define EDITUSERDIALOG_H

#include "model/data.h"

#include <QWidget>
#include <QDialog>

namespace Ui {
class EditUserDialog;
}

class EditUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditUserDialog(QWidget *parent, const QString &text, model::AdminInfo& adminInfo);
    ~EditUserDialog();

    // 确认⽤⼾是否提交
    bool getCommitResult()const;

    // 设置邮箱编辑框是否只读
    void setEmaiEditReadOnly(bool isReadOnly = true);

private slots:
    // 取消按钮点击
    void onCancelBtnClicked();

    // 提交按钮
    void onSubmitBtnClicked();

private:
    Ui::EditUserDialog *ui;

    // 保存管理员信息
    model::AdminInfo &adminInfo;
    // 确认是否提交
    bool isCommit = false;
};

#endif // EDITUSERDIALOG_H
