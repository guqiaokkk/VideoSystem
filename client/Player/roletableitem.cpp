#include "roletableitem.h"
#include "ui_roletableitem.h"

#include "edituserdialog.h"
#include "./model/datacenter.h"
#include "toast.h"

RoleTableItem::RoleTableItem(QWidget *parent, model::AdminInfo& adminInfo, int seqNumber)
    : QWidget(parent)
    , ui(new Ui::RoleTableItem)
    , adminInfo(adminInfo)
{
    ui->setupUi(this);

    initStyleSheet();

    // 更新界⾯UI
    updateUI(seqNumber);

    // 编辑按钮点击
    connect(ui->editBtn, &QPushButton::clicked, this, &RoleTableItem::onEditBtnClicked);

    // 编辑管理员成功
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::editAdminDone, this, [=](){
        ui->emailLabel->setText(this->adminInfo.email);
        ui->nameLabel->setText(this->adminInfo.nickName);

        // 状态
        if(model::enable == this->adminInfo.userStatus)
        {
            // 启⽤
            ui->statusButton->setText("禁用");
        }
        else if(model::disable == this->adminInfo.userStatus)
        {
            // 禁⽤
            ui->statusButton->setText("启用");
        }

        // 修改状态按钮的样式
        ui->statusButton->setStyleSheet(styleSheet[ui->statusButton->text()]);
        ui->commentLabel->setText(this->adminInfo.remark);
    });

    // 状态按钮点击
    connect(ui->statusButton, &QPushButton::clicked, this, &RoleTableItem::onStatusButtonClicked);

    // 删除按钮点击
    connect(ui->delBtn, &QPushButton::clicked, this, &RoleTableItem::onDelBtnClicked);
}

RoleTableItem::~RoleTableItem()
{
    delete ui;
}

void RoleTableItem::updateUI(int seqNumber)
{
    if(seqNumber != 0)
    {
        ui->idLabel->setText(QString::number(seqNumber));   // 序号
    }

    // ⽤⼾⻆⾊
    if(model::Admin == adminInfo.roleType)
    {
        ui->roleLabel->setText("平台管理员");
    }
    else if(model::SuperAdmin == adminInfo.roleType)
    {
        ui->roleLabel->setText("超级管理员");
        // 超级管理员⾝份，没有管理编辑和删除操作，只显⽰⼀个按钮，点击操作禁⽤
        ui->delBtn->hide();
        ui->editBtn->move(1115,25);
        ui->editBtn->setText("--");
        ui->editBtn->setStyleSheet(styleSheet["--"]);
        ui->editBtn->setEnabled(false);
    }

    ui->emailLabel->setText(adminInfo.email);
    ui->nameLabel->setText(adminInfo.nickName);

    // 状态
    if(model::enable == adminInfo.userStatus)
    {
        ui->statusButton->setText("禁用");
    }
    else if(model::disable == adminInfo.userStatus)
    {
        ui->statusButton->setText("启用");
    }
    ui->statusButton->setStyleSheet(styleSheet[ui->statusButton->text()]);

    // 超级管理员不允许启⽤或禁⽤，按钮禁⽤掉
    if(adminInfo.roleType == model::SuperAdmin)
    {
        ui->statusButton->setEnabled(false);
    }

    ui->commentLabel->setText(adminInfo.remark);
}

void RoleTableItem::onEditBtnClicked()
{
    // 在编辑⻚⾯修改管理员信息
    EditUserDialog *editUserDlg = new EditUserDialog(nullptr, "编辑后台⽤⼾", adminInfo);

    editUserDlg->setEmaiEditReadOnly(true); // 调用函数，将邮箱输入框设为只读

    editUserDlg->exec();
    // 如果⽤⼾点击了确认，向服务器发送编辑⽤⼾请求
    if(editUserDlg->getCommitResult())
    {
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->editAdminAsync(adminInfo);
    }
    delete editUserDlg;
}

void RoleTableItem::onStatusButtonClicked()
{
    if(logUserisAdminMyself("⽆法启⽤或禁⽤⾃⼰"))
    {
        return;
    }
    if(adminInfo.roleType == model::RoleType::SuperAdmin)
    {
        Toast::showMessage("不能启⽤或禁⽤超级管理员!!!");
        return;
    }

    QString btnText = ui->statusButton->text();
    if(btnText == "启用")
    {
        btnText = "禁用";
        adminInfo.userStatus = model::enable;
    }
    else
    {
        btnText = "启用";
        adminInfo.userStatus = model::disable;
    }
    ui->statusButton->setText(btnText);
    ui->statusButton->setStyleSheet(styleSheet[btnText]);

    auto dataCenter = model::DataCenter::getInstance();
    auto adminListPtr = dataCenter->getAdminList();

    adminListPtr->setAdminStatus(adminInfo.userId, adminInfo.userStatus);

    // 修改⽤⼾状态
    dataCenter->setAdminStatusAsync(adminInfo);
}

void RoleTableItem::onDelBtnClicked()
{
    if(logUserisAdminMyself("禁⽌删除⾃⼰"))
    {
        return;
    }
    if(adminInfo.roleType == model::RoleType::SuperAdmin)
    {
        Toast::showMessage("不能删除超级管理员!!!");
        return;
    }

    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->delAdminAsync(adminInfo.userId);
}

bool RoleTableItem::logUserisAdminMyself(const QString &text)
{
    auto dataCenter = model::DataCenter::getInstance();
    auto myself = dataCenter->getMyselfInfo();
    if(adminInfo.userId == myself->userId)
    {
        Toast::showMessage(text);
        return true;
    }
    return false;
}

void RoleTableItem::initStyleSheet()
{
    styleSheet.insert("启用", "border:none;"
                              "background-color:#EBF3FF;"
                              "border-radius:10px;"
                              "border:1px solid #EBEDF0;"
                              "font-size:12px;"
                              "color:#3686FF;");
    styleSheet.insert("禁用", "border:none;"
                              "background-color:#FFF0F0;"
                              "border-radius:10px;"
                              "border:1px solid #EBEDF0;"
                              "font-size:12px;"
                              "color:#FD6A6A;");
    styleSheet.insert("--", "border:none;"
                            "background-color:#FFFFFF;"
                            "border-radius:10px;"
                            "font-size:14px;"
                            "color:#222222;");
}




