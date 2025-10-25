#include "playerpage.h"
#include "ui_playerpage.h"

PlayerPage::PlayerPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerPage)
{
    ui->setupUi(this);

    // 实例化⾳量调节窗⼝指针
    volume = new Volume(this);

    // 实例化倍数播放窗⼝对象
    playSpeed = new PlaySpeed(this);

    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_ShowModal);

    connect(ui->minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->quitBtn, &QPushButton::clicked, this, &QWidget::close);

    // 显⽰⾳量调节窗⼝
    connect(ui->volumeBtn, &QPushButton::clicked, this, &PlayerPage::onVolumeBtnClicked);

    // 绑定倍数播放按钮信号槽
    connect(ui->speedBtn, &QPushButton::clicked, this, &PlayerPage::onSpeedBtnClicked);
}

PlayerPage::~PlayerPage()
{
    delete ui;
}

void PlayerPage::onVolumeBtnClicked()
{
    //获取播放窗口左上角的坐标，并将其转换为基于屏幕的坐标
    // QPoint(0, 0)为PlayerPage窗⼝的左上⻆, mapToGlobal(QPoint(0, 0))将PlayerPage窗⼝左上⻆转换为全局坐标
    moveWindows(mapToGlobal(QPoint(0,0)));

    volume->show();
}

void PlayerPage::moveWindows(const QPoint &point)
{
    // point 已是基于屏幕的全局坐标，就是已经到playerpage的左上角了

    // QPoint(this->width() - volume->width()-13, 533)  QPoint为基于播放窗口playerpage的偏移量
    // 13是⾳量调节窗⼝右边界和 PlayerPage的右边界的距离,533是⾳量调节窗⼝距离PlayerPage上边界的距离
    // newPoint最终为⾳量调节窗⼝在PlayerPage中的位置转化的全局坐标
    QPoint newPoint = point + QPoint(this->width() - volume->width() -13, 533);
    volume->move(newPoint);


    // 倍速播放窗⼝位置调整
    newPoint = point + QPoint(this->width() - playSpeed->width() - 77, 563);
    playSpeed->move(newPoint);
}

void PlayerPage::onSpeedBtnClicked()
{
    moveWindows(mapToGlobal(QPoint(0,0)));
    // 弹出倍速窗⼝
    playSpeed->show();
}
