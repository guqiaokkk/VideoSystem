#include "roletable.h"
#include "ui_roletable.h"

#include "util.h"
#include "roletableitem.h"
#include "edituserdialog.h"

RoleTable::RoleTable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoleTable)
{
    ui->setupUi(this);

    ui->userStatus->addItem("全部分类");
    ui->userStatus->addItem("启⽤");
    ui->userStatus->addItem("停⽤");
    ui->userStatus->setCurrentIndex(0);

    // 限制编辑框只能输⼊⼿机号
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->phone->setValidator(validator);

    // 初始化重置和查询按钮的样式
    initStyleSheet();

    // 更新表格元素
    updateRoleTable();

    connect(ui->resetBtn, &QPushButton::clicked, this, &RoleTable::onResetBtnClicked);
    connect(ui->queryBtn, &QPushButton::clicked, this, &RoleTable::onQueryBtnClicked);

    connect(ui->insertBtn, &QPushButton::clicked, this, &RoleTable::onInsertBtnClicked);

    // 创建分⻚器并显⽰
    paginator = new Paginator(10, ui->PaginatorArea);
    paginator->move(0, 20);
    paginator->show();
}

RoleTable::~RoleTable()
{
    delete ui;
}

void RoleTable::onResetBtnClicked()
{
    // 设置按钮样式
    ui->resetBtn->setStyleSheet(styleSheet["选中"]);
    ui->queryBtn->setStyleSheet(styleSheet["未选中"]);
    ui->phone->setText("");
    ui->userStatus->setCurrentIndex(0);
    LOG()<<"重置按钮点击...";
}

void RoleTable::onQueryBtnClicked()
{
    // 设置按钮样式
    ui->queryBtn->setStyleSheet(styleSheet["选中"]);
    ui->resetBtn->setStyleSheet(styleSheet["未选中"]);
    LOG()<<"查询按钮点击...";
}

void RoleTable::updateRoleTable()
{
    // 添加RoleTableItem到表中
    for (int i = 0; i < 10; i++)
    {
        RoleTableItem* item = new RoleTableItem(this, i+1);
        ui->layout->addWidget(item);
    }
}

void RoleTable::onInsertBtnClicked()
{
    // 显⽰新增对后台⽤⼾对话框
    EditUserDialog *editUserDialog = new EditUserDialog(nullptr, "新增后台⽤⼾");
    editUserDialog->exec();
    delete editUserDialog;
}


void RoleTable::initStyleSheet()
{
    styleSheet.insert("选中", "background-color:#3ECEFF;"
                              "border-radius:4px;"
                              "font-family:微软雅⿊;"
                              "font-size:14px;"
                              "color:#FFFFFF;");

    styleSheet.insert("未选中", "background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#222222;");
}
