#include "checktable.h"
#include "ui_checktable.h"

#include "util.h"

#include "checktableitem.h"

CheckTable::CheckTable(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CheckTable)
{
    ui->setupUi(this);
    ui->videoStatus->addItem("全部分类");
    ui->videoStatus->addItem("待审核");
    ui->videoStatus->addItem("审核通过");
    ui->videoStatus->addItem("审核驳回");
    ui->videoStatus->addItem("已下架");
    ui->videoStatus->addItem("转码中");
    ui->videoStatus->setCurrentIndex(0);

    //使⽤正则表达式对视频⽤⼾id进⾏校验
    //eg:7d98-a3846253-0003
    QRegularExpression regExp("^[0-9a-f]{4}-[0-9a-f]{8}-[0-9a-f]{4}$");
    QValidator *validator = new QRegularExpressionValidator(regExp, this);
    // 将创建的验证器设置到ui->userIdEdit上
    ui->userIdEdit->setValidator(validator);


    // 重置按钮点击信号槽绑定
    connect(ui->resetBtn, &QPushButton::clicked, this, &CheckTable::onResetBtnClicked);

    // 查询按钮点击信号槽绑定
    connect(ui->queryBtn, &QPushButton::clicked, this, &CheckTable::onQueryBtnClicked);

    // 默认显⽰审核⻚⾯
    updateCheckTable();


    // 创建分⻚器并显⽰
    paginator = new Paginator(10, ui->PaginatorArea);
    paginator->move(0, 15);
    paginator->show();
}


CheckTable::~CheckTable()
{
    delete ui;
}

void CheckTable::updateCheckTable()
{
    // 将CheckTableItem添加到界⾯中
    for (int i = 0; i < 10; i++)
    {
        CheckTableItem* item = new CheckTableItem(this);
        ui->layout->addWidget(item);
    }
}

void CheckTable::onResetBtnClicked()
{
    // 设置按钮样式
    ui->resetBtn->setStyleSheet("background-color:#3ECEFF;"
                                "border-radius:4px;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#FFFFFF;");

    ui->queryBtn->setStyleSheet("background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#222222;");

    // 清空⽤⼾id
    ui->userIdEdit->setText("");
    ui->videoStatus->setCurrentIndex(0);
    LOG()<<"重置按钮点击...";
}

void CheckTable::onQueryBtnClicked()
{
    ui->queryBtn->setStyleSheet("background-color:#3ECEFF;"
                                "border-radius:4px;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#FFFFFF;");

    ui->resetBtn->setStyleSheet("background:#FFFFFF;"
                                "border-radius:4px;"
                                "border:1px solid #DCDEE0;"
                                "font-family:微软雅⿊;"
                                "font-size:14px;"
                                "color:#222222;");
    LOG()<<"查询按钮点击...";
}
