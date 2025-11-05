#ifndef MODIFYMYSELFDIALOG_H
#define MODIFYMYSELFDIALOG_H

#include <QWidget>
#include <QDialog>


namespace Ui {
class ModifyMyselfDialog;
}

class ModifyMyselfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyMyselfDialog(QWidget *parent = nullptr);
    ~ModifyMyselfDialog();


private slots:
    void onSubmitBtnClicked();
    void onCancelBtnClicked();
    void showPasswordDlg();

private:
    Ui::ModifyMyselfDialog *ui;

    // 新修改的密码. 通过 NewPasswordDialog 传进来的
    QString newPassword;
};

#endif // MODIFYMYSELFDIALOG_H
