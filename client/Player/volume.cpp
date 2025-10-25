#include "volume.h"
#include "ui_volume.h"

Volume::Volume(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Volume)
{
    ui->setupUi(this);

    // 将⾳量调节窗⼝设置为弹出窗⼝
    setWindowFlag(Qt::Popup);

    ui->volumeBox->installEventFilter(this);
}

Volume::~Volume()
{
    delete ui;
}

bool Volume::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->volumeBox == watched){
        //press
        if(event->type() == QEvent::MouseButtonPress){
            calcVolume();
        }
        //release
        else if(event->type() == QEvent::MouseButtonRelease){
            ;
        }
        //move
        else if(event->type() == QEvent::MouseMove){
            calcVolume();
        }
        return true;
    }

    return QObject::eventFilter(watched, event);
}

void Volume::calcVolume()
{
    // 获取⿏标点击时的 y 坐标
    //QCursor().pos()⿏标点击时的位置，该位置是基于屏幕的全局坐标
    //mapFromGlobal 将全局坐标转换为窗口内的坐标
    int y = ui->volumeBox->mapFromGlobal(QCursor().pos()).y();

    // 验证y的正确性，y的范围在[5, 150]之间
    y = y < 5 ? 5 : y;
    y = y > 150 ? 150 : y;

    // 更新outLine的⾼度
    ui->outLine->setGeometry(ui->outLine->x(), y, ui->outLine->width(), 150-y);

    // 更新volumeBtn的位置
    int vlBtn = ui->outLine->y() - ui->volumeBtn->height() / 2;
    // 为了防⽌当outLine的y坐标为150时，按钮的⼀半显⽰不出来的问题
    if(vlBtn > 140){
        vlBtn = 140;
    }
    ui->volumeBtn->move(ui->volumeBtn->x(), vlBtn);
}
