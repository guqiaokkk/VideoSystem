#include "edituserdialog.h"
#include "ui_edituserdialog.h"

#include "player.h"
#include "./model/datacenter.h"
#include "toast.h"

EditUserDialog::EditUserDialog(QWidget *parent, const QString& text, model::AdminInfo& adminInfo)
    : QDialog(parent)
    , ui(new Ui::EditUserDialog)
    , adminInfo(adminInfo)
{
    ui->setupUi(this);

    // 去除窗⼝边框
    setWindowFlag(Qt::FramelessWindowHint);
    // 设置窗⼝透明
    setAttribute(Qt::WA_TranslucentBackground);
    // 设置窗⼝描述
    ui->titleLabel->setText(text);

    // 将窗⼝位置移动到和主窗⼝左上⻆重合
    Player *player = Player::getInstance();
    this->move(player->mapToGlobal(QPoint(0, 0)));

    // 取消按钮点击
    connect(ui->cancelBtn, &QPushButton::clicked, this, &EditUserDialog::onCancelBtnClicked);


    // 对⼿机号编辑框需进⾏限制：以1开始的11位整形数字
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->emailEdit->setValidator(validator);

    // 设置管理员⻆⾊
    ui->roleComboBox->addItem("平台管理员");
    ui->roleComboBox->setCurrentIndex(0);

    // 如果id不空，说明是编辑⽤⼾，否则为新增⽤⼾
    if(!adminInfo.userId.isEmpty())
    {
        //编辑⽤⼾
        ui->emailEdit->setText(adminInfo.email);
        // 如果⽬前⻆⾊只有⼀个
        if(adminInfo.roleType == model::Admin)
        {
            ui->roleComboBox->setCurrentIndex(0);
        }
        ui->nameEdit->setText(adminInfo.nickName);

        // 根据用户的状态，勾选对应的单选按钮
        if(adminInfo.userStatus == model::enable)
        {
            ui->startRadioBtn->setChecked(true);
        }
        else
        {
            ui->stopRadioBtn->setChecked(true);
        }
        ui->commentTextEdit->setPlainText(adminInfo.remark);
    }

    // 编辑框内容变化信号
    connect(ui->commentTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        QString text = ui->commentTextEdit->toPlainText();
        int wordCount = text.size();
        if(wordCount <= 10){
            ui->wordCount->setText(QString::number(wordCount) + "/10");
        }else{
            ui->commentTextEdit->setPlaceholderText(text.mid(0,10));
        }
    });


    connect(ui->submitBtn, &QPushButton::clicked, this, &EditUserDialog::onSubmitBtnClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &EditUserDialog::onCancelBtnClicked);
}

EditUserDialog::~EditUserDialog()
{
    delete ui;
}

bool EditUserDialog::getCommitResult() const
{
    return isCommit;
}

void EditUserDialog::setEmaiEditReadOnly(bool isReadOnly)
{
    ui->emailEdit->setReadOnly(true);
}

void EditUserDialog::onCancelBtnClicked()
{
    isCommit = false;
    close();
}


void EditUserDialog::onSubmitBtnClicked()
{
    adminInfo.email = ui->emailEdit->text();

    // ⻆⾊只能选择平台管理员
    adminInfo.roleType = model::Admin;
    adminInfo.nickName = ui->nameEdit->text();

    // 判断用户选择的状态（启用/停用）
    model::AdminStatus adminStatus = model::enable;
    if(!ui->startRadioBtn->isChecked())
    {
        adminStatus = model::disable;
    }

    // 注意：禁⽌管理员⾃⼰禁⽤⾃⼰
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    // (正在编辑的用户ID == 当前登录的管理员ID) 并且 (用户在界面上选择的新状态 != 该用户原来的状态)
    if(myselfInfo->userId == adminInfo.userId && adminStatus != adminInfo.userStatus)
    {
        Toast::showMessage("禁⽌管理员启⽤或禁⽤⾃⼰");
        return;
    }

    adminInfo.userStatus = adminStatus;
    adminInfo.remark = ui->commentTextEdit->toPlainText();
    isCommit = true;
    close();
}
