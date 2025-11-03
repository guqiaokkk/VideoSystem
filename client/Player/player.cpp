#include "player.h"
#include "./ui_player.h"

#include "util.h"

#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>




// 初始化实例指针，并获取实例
Player *Player::instance = nullptr;

Player::Player(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Player)
{
    ui->setupUi(this);

    initUI();

    // 绑定信号槽
    connectSignalAndSlot();
}

Player::~Player()
{
    delete ui;
}

Player *Player::getInstance()
{
    if(instance == nullptr){
        instance = new Player();
    }
    return instance;
}

void Player::showSystemPageBtn(bool isShow)
{
    if(isShow)
    {
        ui->sysPageBtn->show();
    }
    else
    {
        ui->sysPageBtn->hide();
    }
}

// 在播放⻚⾯时，点击⽤⼾图像查看userid信息
void Player::switchToUserInfoPage(const QString &userId)
{
    onSwitchPageUI(MyselfPage);

    // 加载其他⽤⼾个⼈信息
    ui->myPage->loadOtherUser(userId);
}

void Player::onSwitchStackedWidgetPage(int pageId)
{
    onSwitchPageUI(pageId);


    // 如果是我的⻚⾯，加载个⼈⽤⼾信息
    if(pageId == MyselfPage)
    {
        ui->myPage->loadMyself();
    }
}

void Player::onSwitchPageUI(int pageId)
{
    ui->stackedWidget->setCurrentIndex(pageId);
    // 切换按钮按下后，需要重置按钮上⽂本和图⽚的⾼亮样式
    resetSwitchBtnInfo(pageId);

    repaint();
}

void Player::initUI()
{
    // 去除窗⼝标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    // 设置窗⼝图标，图标显⽰在窗⼝标题栏和任务栏上
    setWindowIcon(QIcon(":/images/homePage/logo.png"));

    // 阴影效果必须要窗⼝透明, 设置窗口透明
    setAttribute(Qt::WA_TranslucentBackground);

    //阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);
    shadowEffect->setBlurRadius(7);
    shadowEffect->setOffset(0, 0);

    // 将阴影设置到主窗⼝的⼦控件上
    ui->PlayerBg->setGraphicsEffect(shadowEffect);

    // 设置bodyLeft中⾃定义按钮⽂本和图⽚
    ui->homePageBtn->setTextColor("#000000");                                               //刚开始默认选择homepage
    ui->homePageBtn->setImageAndText(":/images/homePage/shouyexuan.png", "⾸页", HomePage);
    ui->myPageBtn->setTextColor("#999999");
    ui->myPageBtn->setImageAndText(":/images/homePage/wode.png", "我的", MyselfPage);
    ui->sysPageBtn->setTextColor("#999999");
    ui->sysPageBtn->setImageAndText(":/images/homePage/admin.png", "系统", AdminPage);

    // 默认情况下，选中⾸⻚，系统管理⻚⾯隐藏
    ui->stackedWidget->setCurrentIndex(0);
}

void Player::connectSignalAndSlot()
{
    //缩小和关闭
    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &QWidget::close);

    // bodyLeft左侧按钮点击，切换bodyRight⻚⾯
    connect(ui->homePageBtn, &PageSwitchButton::switchPage, this, &Player::onSwitchStackedWidgetPage);
    connect(ui->myPageBtn, &PageSwitchButton::switchPage, this, &Player::onSwitchStackedWidgetPage);
    connect(ui->sysPageBtn, &PageSwitchButton::switchPage, this, &Player::onSwitchStackedWidgetPage);

    // 从我的⻚⾯切换到上传视频⻚⾯
    connect(ui->myPage, &MyselfWidget::switchUploadVideoPage, this, [=](int pageId){
        LOG()<<"切换到上传视频⻚⾯";
        onSwitchStackedWidgetPage(pageId);
    });
    // 视频上传⻚⾯切换到我的⻚⾯
    connect(ui->uploadVideoPage, &UploadVideoPage::switchMyselfPage, this, [=](int pageId){
        onSwitchStackedWidgetPage(pageId);
    });
}



void Player::resetSwitchBtnInfo(int pageId)
{
    // 重新设置左侧栏未选中按钮颜⾊
    QList<PageSwitchButton*> switchBtns = findChildren<PageSwitchButton*>();
    for(auto switchBtn : switchBtns){
        if(switchBtn->getPageId() != pageId){
            switchBtn->setTextColor("#999999");
        }
    }
    // 重新设置按钮上图⽚
    if(pageId == HomePage){
        ui->homePageBtn->setImage(":/images/homePage/shouyexuan.png");
        ui->myPageBtn->setImage(":/images/homePage/wode.png");
        ui->sysPageBtn->setImage(":/images/homePage/admin.png");
    }
    else if(pageId == MyselfPage){
        ui->homePageBtn->setImage(":/images/homePage/shouye.png");
        ui->myPageBtn->setImage(":/images/homePage/wodexuan.png");
        ui->sysPageBtn->setImage(":/images/homePage/admin.png");
    }
    else if(pageId == AdminPage){
        ui->homePageBtn->setImage(":/images/homePage/shouye.png");
        ui->myPageBtn->setImage(":/images/homePage/wode.png");
        ui->sysPageBtn->setImage(":/images/homePage/adminxuan.png");
    }
    else{
        qDebug()<<"暂不⽀持⻚⾯："<<pageId;
    }
}

void Player::mousePressEvent(QMouseEvent *event)
{
    // 获取⿏标按下时的位置
    QPoint point = event->position().toPoint();

    // 如果⿏标按下位置在标题栏，即head区域则进⾏拖拽
    if(ui->head->geometry().contains(point)){
        if(event->button() == Qt::LeftButton){
            //记录⿏标按下位置和窗⼝左上⻆的相对距离
            dragpos = event->globalPosition().toPoint() - geometry().topLeft();
            return;
        }
    }
    QWidget::mousePressEvent(event);
}

void Player::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->position().toPoint();
    if(ui->head->geometry().contains(point)){
        if(event->buttons() == Qt::LeftButton){
            // 当⿏标移动时，保证⿏标位置相对于窗⼝左上⻆位置不变，则需要move窗⼝的位置
            move(event->globalPosition().toPoint() - dragpos);
        }
    }
}
