#include "adminwidget.h"
#include "ui_adminwidget.h"

AdminWidget::AdminWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdminWidget)
{
    ui->setupUi(this);

    // 初始化界⾯为审核状态
    onCheckBtnClicked();

    // 审核管理按钮
    connect(ui->checkBtn, &QPushButton::clicked, this, &AdminWidget::onCheckBtnClicked);

    // ⻆⾊管理按钮
    connect(ui->roleBtn, &QPushButton::clicked, this, &AdminWidget::onRoleBtnClicked);
}

AdminWidget::~AdminWidget()
{
    delete ui;
}

void AdminWidget::onCheckBtnClicked()
{
    ui->checkBtn->setStyleSheet(activeTabStyle);
    ui->roleBtn->setStyleSheet(inactiveTabStyle);
    ui->checkTable->show();
    ui->roleTable->hide();
}

void AdminWidget::onRoleBtnClicked()
{
    ui->roleBtn->setStyleSheet(activeTabStyle);
    ui->checkBtn->setStyleSheet(inactiveTabStyle);
    ui->roleTable->show();
    ui->checkTable->hide();
}
