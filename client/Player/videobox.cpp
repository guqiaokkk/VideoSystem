#include "videobox.h"
#include "ui_videobox.h"

#include "util.h"

VideoBox::VideoBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoBox)
{
    ui->setupUi(this);

    // 默认隐藏删除按钮
    ui->delVideoBtn->hide();

    // 给VideoBox安装事件拦截器，让视频封⾯和标题控件的⿏标点击事件
    ui->imgBox->installEventFilter(this);
    ui->videoTitle->installEventFilter(this);

    playPage = new PlayerPage();
}

VideoBox::~VideoBox()
{
    delete ui;
}

bool VideoBox::eventFilter(QObject *watched, QEvent *event)
{
    // 拦截视频封⾯和视频标题的⿏标点击事件
    if(ui->imgBox == watched || ui->videoTitle == watched)
    {
        if(QEvent::MouseButtonPress == event->type())
        {
            onPlayClicked();
            return true;
        }
    }
    return QObject::eventFilter(watched, event);
}

void VideoBox::onPlayClicked()
{
    playPage->show();
}
