#include "modifymyselfdialog.h"
#include "ui_modifymyselfdialog.h"

#include "util.h"

#include "newpassworddialog.h"

ModifyMyselfDialog::ModifyMyselfDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModifyMyselfDialog)
{
    ui->setupUi(this);

    // 去掉窗⼝边框
    setWindowFlag(Qt::FramelessWindowHint);
    ui->passwordWidget->hide();

    connect(ui->submitBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::onSubmitBtnClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::onCancelBtnClicked);
    connect(ui->passwordBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::showPasswordDlg);
    connect(ui->changePasswordBtn, &QPushButton::clicked, this, &ModifyMyselfDialog::showPasswordDlg);
}

ModifyMyselfDialog::~ModifyMyselfDialog()
{
    delete ui;
}

void ModifyMyselfDialog::onSubmitBtnClicked()
{
    LOG() << "submit";
}

void ModifyMyselfDialog::onCancelBtnClicked()
{
    close();
}

void ModifyMyselfDialog::showPasswordDlg()
{
    // 显⽰再次修改密码对话框, ⽤主窗⼝作为对话框的⽗窗⼝.
    NewPasswordDialog *dialog = new NewPasswordDialog();
    dialog->exec();

    // 获取修改后密码
    // 引⼊⼀个临时变量 currentPassword, 因为当对话框dialog如果复用，会出现这样的问题:
    // 1. ⽤⼾点击修改密码, 弹出修改密码对话框, 输⼊密码, 确定。 该函数内的dialog里的QString newPassword成员就被设置了
    // 2. ⽤⼾再次点击修改密码, 弹出修改密码对话框, 输⼊密码, 但突然不想修改了，点击取消，他会返回一个空字符串
    //    如果是直接把从修改密码对话框读出的密码设置到 newPassword, 那么随着第⼆次的取消动作, 就会使第⼀次修改的值也被取消掉了.
    //    所以使用临时变量接收结果，一开始默认为空，只有在用户确认修改时才更新状态，取消操作不影响现有状态

    //此处其实不会出现状态污染的问题

    QString currenPassword = dialog->getNewPassword();
    if(currenPassword.isEmpty())
    {
        LOG() << "取消修改密码";
        return;
    }

    // 隐藏修改密码按钮, 显⽰已修改
    ui->passwordBtn->hide();
    ui->passwordWidget->show();


    delete dialog;
}
