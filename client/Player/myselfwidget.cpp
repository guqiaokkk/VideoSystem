#include "myselfwidget.h"
#include "ui_myselfwidget.h"

#include "modifymyselfdialog.h"
#include "util.h"
#include "player.h"

#include <QFileDialog>

MyselfWidget::MyselfWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyselfWidget)
{
    ui->setupUi(this);

    initUI();

    // 头像按钮绑定槽函数
    connect(ui->avatarBtn, &QPushButton::clicked, this, &MyselfWidget::uploadAvatarBtnClicked);

    // 上传视频的绑定
    connect(ui->uploadVideoBtn, &QPushButton::clicked, this, &MyselfWidget::uploadViewBtnClicked);
}

MyselfWidget::~MyselfWidget()
{
    delete ui;
}

void MyselfWidget::initUI()
{
    ui->attentionBtn->hide();

    // 设置按钮绑定槽函数
    connect(ui->settingBtn, &QPushButton::clicked, this, &MyselfWidget::settingBtnClicked);
}

void MyselfWidget::uploadAvatarBtnClicked()
{
    // 1.弹出对话框, 选择⽂件
    QString filename = QFileDialog::getOpenFileName(nullptr, "选择头像", "", "Image Files (*.jpg *.png)");
    if(filename.isEmpty()){
        LOG() << "取消选择头像";
        return;
    }

    // 2.读取文件到fileData
    QByteArray fileData = loadFileToByteArray(filename);
    if (fileData.isEmpty())
    {
        LOG() << "头像⽂件读取失败";
        return;
    }

    // 3.通过makeIcon将fileData转换为QIcon，并设为头像
    // 按钮的尺⼨为60*60，因此圆形按钮的半径为按钮⻓或宽的⼀半
    ui->avatarBtn->setIcon(makeIcon(fileData, ui->avatarBtn->width()/2));
}

void MyselfWidget::settingBtnClicked()
{
    // 弹出对话框设置⽤⼾信息
    ModifyMyselfDialog *dialog = new ModifyMyselfDialog();
    dialog->exec();
    delete dialog;
}

void MyselfWidget::uploadViewBtnClicked()
{
    // 1. 弹出打开⽂件对话框，让⽤⼾选择要上传的视频⽂件
    QString videoFilePath = QFileDialog::getOpenFileName(nullptr, "上传视频", "", "Videos (*.mp4 *.rmvb *.avi *.mov)");
    if(!videoFilePath.isEmpty())
    {
        // 视频⼤⼩限制，上限为4G
        QFileInfo fileInfo(videoFilePath);
        int64_t fileSize = fileInfo.size();
        if(fileSize > (int64_t)4 * 1024 * 1024 * 1024)
        {
            LOG()<<"视频⽂件必须⼩于4G";
            return;
        }

        emit switchUploadVideoPage(UploadPage);
    }
}
