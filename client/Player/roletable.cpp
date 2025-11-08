#include "roletable.h"
#include "ui_roletable.h"

#include "./model/datacenter.h"
#include "util.h"
#include "roletableitem.h"
#include "edituserdialog.h"
#include "toast.h"

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
    ui->email->setValidator(validator);

    // 初始化重置和查询按钮的样式
    initStyleSheet();

    // 更新表格元素
    updateRoleTable();

    // 创建分⻚器并显⽰
    paginator = new Paginator(10, ui->PaginatorArea);
    paginator->move(0, 20);
    paginator->show();

    // ⽔平布局器中的元素向上对⻬
    ui->layout->setAlignment(Qt::AlignTop);

    connect(ui->resetBtn, &QPushButton::clicked, this, &RoleTable::onResetBtnClicked);
    connect(ui->queryBtn, &QPushButton::clicked, this, &RoleTable::onQueryBtnClicked);

    connect(ui->insertBtn, &QPushButton::clicked, this, &RoleTable::onInsertBtnClicked);

    // 获取管理员信息信号槽-邮箱
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getAdminByEmailDone, this, &RoleTable::updateRoleTable);

    // 获取管理员信息信号槽-状态
    connect(dataCenter, &model::DataCenter::getAdminListByStatusDone, this, &RoleTable::updateRoleTable);

    // 新增管理员完成
    connect(dataCenter, &model::DataCenter::newAdminDone, this, [=](){
        // 新增管理员成功，重新获取下管理员列表
        getAdminList(1);
    });

    // 删除管理员成功
    connect(dataCenter, &model::DataCenter::delAdminDone, this, [=](){
        // 重新获取管理员列表，界⾯和DataCenter中数据都可以更新
        getAdminList(1);
    });
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
    ui->email->setText("");
    ui->userStatus->setCurrentIndex(0);
    // 获取管理员列表
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isAdminDisable())
    {
        Toast::showMessage("您已被禁⽌，⽆法进⾏操作");
    }
    else
    {
        getAdminList(1);
    }
}

void RoleTable::onQueryBtnClicked()
{
    // 设置按钮样式
    ui->queryBtn->setStyleSheet(styleSheet["选中"]);
    ui->resetBtn->setStyleSheet(styleSheet["未选中"]);
    // 获取管理员列表
    auto dataCenter = model::DataCenter::getInstance();
    auto myselfInfo = dataCenter->getMyselfInfo();
    if(myselfInfo->isAdminDisable())
    {
        Toast::showMessage("您已被禁⽌，⽆法进⾏操作");
    }
    else
    {
        getAdminList(1);
    }
}

void RoleTable::updateRoleTable()
{
    // 1. 清空界面中旧的管理员信息
    QLayoutItem *item;
    while ((item = ui->layout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

    // 2. 获取管理员列表
    auto dataCenter = model::DataCenter::getInstance();
    auto adminInfoList = dataCenter->getAdminList();
    if(adminInfoList == nullptr)
    {
        return;
    }

    // 3. 重置分页器
    auto adminList = adminInfoList->adminList;
    int adminCountPage = model::AdminList::PAGE_COUNT;
    int totalCount = adminInfoList->totalCount;
    if(page == 1)
    {
        resetPaginator((totalCount + adminCountPage-1) / adminCountPage);
    }

    // 4. 将管理员信息添加到界面中
    int seqNum = (page-1) * adminCountPage + 1;
    for(int i = 0;i < adminList.size(); ++i)
    {
        RoleTableItem *item = new RoleTableItem(this, adminList[i], seqNum++);
        ui->layout->addWidget(item);
    }
}

void RoleTable::onInsertBtnClicked()
{
    // 显⽰新增对后台⽤⼾对话框
    model::AdminInfo adminInfo;
    EditUserDialog *editUserDialog = new EditUserDialog(nullptr, "新增后台⽤⼾", adminInfo);
    editUserDialog->exec();

    // 如果⽤⼾点击了确认按钮，此处需要将新添加的管理员信息上传到服务器
    if(editUserDialog->getCommitResult())
    {
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->newAdminAsync(adminInfo);
    }
    delete editUserDialog;
}

void RoleTable::resetPaginator(int pageCount)
{
    // 当重新获取管理员列表后，每次获取结果的⻚⾯都不⼀样，分⻚器重新设置下
    if(paginator)
    {
        delete paginator;
    }

    paginator = new Paginator(pageCount, ui->PaginatorArea);
    paginator->move(0, 15);
    paginator->show();

    // 分⻚器信号
    connect(paginator, &Paginator::pageChanged, this, [=](int page){
        getAdminList(page);
    });
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

void RoleTable::getAdminList(int page)
{
    this->page = page;
    auto dataCenter = model::DataCenter::getInstance();

    // 现将之前的管理员列表信息清空
    auto adminListPtr = dataCenter->getAdminList();
    adminListPtr->adminList.clear();

    // 优先按照邮箱获取管理员信息
    QString email = ui->email->text();
    if(!email.isEmpty())
    {
        if(email.length() != 11)
        {
            Toast::showMessage("输⼊邮箱有误，请重新输⼊");
            return;
        }
        dataCenter->getAdminByEmailAsync(email);
    }
    else
    {
        // 通过状态获取管理员信息
        model::AdminStatus videoStatue = static_cast<model::AdminStatus>(ui->userStatus->currentIndex());
        dataCenter->getAdminListByStatus(page, videoStatue);
    }

}
