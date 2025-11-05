#include "uploadvideopage.h"
#include "ui_uploadvideopage.h"
#include "player.h"
#include "util.h"
#include <QFileDialog>
#include "./model/datacenter.h"
#include "util.h"
#include "./mpv/mpvplayer.h"
#include "toast.h"

#include <QFileDialog>


UploadVideoPage::UploadVideoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UploadVideoPage)
{
    ui->setupUi(this);

    // 获取所有分类，并更新到界⾯
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTag = dataCenter->getKindAndTagsClassPtr();
    ui->kinds->addItems(kindAndTag->getAllKinds());
    ui->kinds->setCurrentIndex(-1); // 默认不选中

    // 默认情况下，上传视频成功图⽚隐藏，视频上传成功后显⽰
    ui->downIcon->hide();

    // 设置标签不拦截鼠标事件
    ui->imgLabel->setAttribute(Qt::WA_TransparentForMouseEvents);




    // 提交按钮
    connect(ui->commitBtn, &QPushButton::clicked, this, &UploadVideoPage::onCommitBtnClicked);

    // LineEdit和PlainTextEdit中内容变化，就会发射textChanged信号
    connect(ui->videoTitle, &QLineEdit::textChanged, this, &UploadVideoPage::onLineEditTextChanged);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &UploadVideoPage::onPlainEditTextChanged);

    connect(ui->changeButton, &QPushButton::clicked, this, &UploadVideoPage::onChangeBtnClicked);

    // 分类选择改变
    connect(ui->kinds, &QComboBox::currentTextChanged, this, &UploadVideoPage::onUpdataTags);

    // 上传视频成功
    connect(dataCenter, &model::DataCenter::uploadVideoDone, this, &UploadVideoPage::onUploadVideoDone);

    // 上传视频封⾯图成功
    connect(dataCenter, &model::DataCenter::uploadPhotoDone, this, [=](const QString &coverImgId, QWidget* wndPtr){
        // 检测是否为上传封⾯图成功
        if(wndPtr != ui->imgLabel)
        {
            return;
        }
        this->coverImgId = coverImgId;
        isUploadPhotoOk = true;
    });

    // 上传视频信息成功
    connect(dataCenter, &model::DataCenter::uploadVideoDescDone, this, [=](){
        // 清空本次上传视频时在界面上的数据
        resetPage();
        // 切换回我的页面
        emit switchMyselfPage(MyselfPage);
    });
}

UploadVideoPage::~UploadVideoPage()
{
    delete ui;
}

// 将视频名称设置到界⾯
void UploadVideoPage::setVideoTitle(const QString &videoFilePath)
{
    this->videoFilePath = videoFilePath;

    // 截取⽂件名设置到界⾯
    int start = videoFilePath.lastIndexOf('/') + 1;
    QString videoTitle = videoFilePath.mid(start);
    ui->videoTitle->setText(videoTitle);
    ui->fileName->setText(videoTitle);
}

void UploadVideoPage::onCommitBtnClicked()
{
    if(!isUploadVideoOk)
    {
        Toast::showMessage("等待视频上传完成");
        return;
    }

    if(!isUploadVideoOk)
    {
        Toast::showMessage("等待视频封⾯图上传完成");
        return;
    }

    if(!isDurationOk)
    {
        Toast::showMessage("等待视频总时⻓获取完成");
        return;
    }

    // 只有图⽚和视频都上传完成后，才能拿到视频和图⽚的id，才能进⾏后续视频信息上传
    if(isUploadVideoOk && isUploadPhotoOk && isDurationOk)
    {
        model::VideoDesc videoDesc;
        videoDesc.videoId = videoId;
        videoDesc.photoId = coverImgId;
        // 获取视频标题
        videoDesc.tittle = ui->videoTitle->text();
        // 获取视频简介
        videoDesc.desc = ui->plainTextEdit->toPlainText();
        // 获取视频分类
        videoDesc.kind = ui->kinds->currentText();
        // 视频总时⻓
        videoDesc.duration = duration;

        // 获取视频标签
        QList<QPushButton*> tagBtns = ui->tagContent->findChildren<QPushButton*>();
        for(auto &tagBtn : tagBtns)
        {
            if(tagBtn->isChecked())
            {
                QString tag = tagBtn->text();
                videoDesc.tags.append(tag);
            }
        }

        if(videoDesc.tags.size() > 5)
        {
            Toast::showMessage("最多只能选择5个标签");
            return;
        }

        // 新增视频信息到服务器
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->uploadVideoDescAsync(videoDesc);
    }
}

void UploadVideoPage::onLineEditTextChanged(const QString &text)
{
    // 获取已经输⼊的⽂本
    QString leftWord = ui->leftWord->text();
    int wordCount = text.size();

    // 从界⾯截取出能容纳的总字数: 0/80, 截取到80
    int linePos = leftWord.indexOf('/');
    QString totalWords = leftWord.mid(linePos + 1);

    // 如果字数不够上限，可以继续输⼊
    if(totalWords.toInt() - wordCount >= 0)
    {
        ui->leftWord->setText(QString::number(wordCount) + "/" + totalWords);
    }
    else
    {
        LOG() << "字数已达上限，不再显示";
        ui->videoTitle->setText(text.mid(0,80));
    }
}

void UploadVideoPage::onPlainEditTextChanged()
{
    // 获取已经输⼊的⽂本
    QString text = ui->plainTextEdit->toPlainText();
    int wordCount = text.size();

    // 从界⾯截取出能容纳的总字数: 0/1000, 截取到1000
    QString leftWord = ui->briefLeftWord->text();
    int linePos = leftWord.indexOf('/');
    QString totalWords = leftWord.mid(linePos + 1);

    // 如果字数不够上限，可以继续输⼊
    if(totalWords.toInt() - wordCount >= 0)
    {
        ui->briefLeftWord->setText(QString::number(wordCount) + '/' + totalWords);
    }
    else
    {
        ui->plainTextEdit->setPlainText(text.mid(0, totalWords.toInt()));

        // 设置滚动条⼀直处于底部
        // textCursor: 获取当前光标的位置 ,movePosition: 移动光标位置
        // QTextCursor::End移动到⽂本的末尾 , QTextCursor::MoveAnchor表⽰移动光标时，光标的位置是固定的，不会创建⼀个选区（即不会选中⽂本）
        QTextCursor textCursor = ui->plainTextEdit->textCursor();
        textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->plainTextEdit->setTextCursor(textCursor);
    }
}

void UploadVideoPage::onChangeBtnClicked()
{
    // 如果上传多个封⾯，使⽤最新的封⾯
    QString coverImgPath = QFileDialog::getOpenFileName(nullptr, "选择视频封⾯图", "", "Images (*.png *.xpm *.jpg)");
    if(!coverImgPath.isEmpty())
    {
        // 将获取到的视频封⾯图显⽰到界⾯
        QPixmap pixmap(coverImgPath);
        pixmap = pixmap.scaled(ui->imgLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->imgLabel->setPixmap(pixmap);
        repaint();
    }

    // ⽤⼾选择了图⽚，则不使⽤默认图⽚
    uploadPhoto(coverImgPath);
}

void UploadVideoPage::onUpdataTags(const QString &kind)
{
    addTagsByKind(kind);
}

void UploadVideoPage::addTagsByKind(const QString &kind)
{
    // 1.添加之前先清空之前的标签
    QList<QPushButton*> tagBtnList = ui->tagWidget->findChildren<QPushButton*>();
    for(auto tagBtn : tagBtnList)
    {
        // 将元素从布局中移除，并不会删除
        ui->tagLayout->removeWidget(tagBtn);
        delete tagBtn;
    }
    // 将添加的弹簧删除掉
    // 弹簧本来是最后⼀个控件，当layout中的按钮删除之后，弹簧就变成第0个控件了
    QLayoutItem *spaceItem = ui->tagLayout->itemAt(ui->tagLayout->count() - 1);
    ui->tagLayout->removeItem(spaceItem);

    // 2. 根据kind获取标签--如果key不存在直接返回
    if(kind.isEmpty()){
        return;
    }
    auto kindAndTagPtr = model::DataCenter::getInstance()->getKindAndTagsClassPtr();
    auto kinds = model::DataCenter::getInstance()->getKindAndTagsClassPtr()->getAllKinds();
    auto tags = kindAndTagPtr->getTagsByKind(kind);

    // 3. 创建该标签对应的按钮
    for(auto &tag : tags.keys())
    {
        QPushButton *tagBtn = new QPushButton(ui->tagContent);
        tagBtn->setFixedSize(98, 49);
        tagBtn->setText(tag);
        tagBtn->setCheckable(true);
        // 设置按钮的状态为选中和未选中两种状态

        // QPushButton:unchecked 当按钮未选中时
        tagBtn->setStyleSheet("QPushButton{"
                              "border : 1px solid #3ECEFE;"
                              "border-radius : 4px;"
                              "color : #3ECEFE;"
                              "font-family : 微软雅⿊;"
                              "font-size : 16px;"
                              "background-color : #FFFFFF;}"
                              "QPushButton:checked{"
                              "background-color : #3ECEFE;"
                              "color : #FFFFFF;}"
                              "QPushButton:unchecked{"
                              "background-color : #FFFFFF;"
                              "color : #3ECEFE;}");
        ui->tagLayout->addWidget(tagBtn);
    }

    // 在tagLayout最后放⼀个空⽩间距，将按钮挤到左侧
    ui->tagLayout->insertSpacing(tags.size(), ui->tagContent->width() - (98+20)*tags.size());
    ui->tagLayout->setSpacing(20);
}


void UploadVideoPage::resetPage()
{
    ui->imgLabel->setStyleSheet("#imgLabel{""border-image : url(:/images/uploadVideoPage/fengmian.png);}");
    ui->videoTitle->setText("");
    ui->plainTextEdit->setPlainText("");
    ui->uploadProgress->setText("上传中...");
    ui->downIcon->hide();

    // 分类和标签默认不选中
    ui->kinds->setCurrentIndex(-1);
    addTagsByKind("");

    isUploadVideoOk = false;
    isUploadPhotoOk = false;
    isDurationOk = false;
    videoId = "";
    coverImgId = "";
    duration = 0;


}

void UploadVideoPage::onUploadVideoDone(const QString &videoId)
{
    ui->uploadProgress->setText("上传完成");
    ui->downIcon->show();
    this->videoId = videoId;
    isUploadVideoOk = true;

    // 设置视频⾸帧
    QString firstFrame = MpvPlayer::getVideoFirstFrame(videoFilePath);
    QPixmap pixmap(firstFrame);
    pixmap = pixmap.scaled(ui->imgLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    ui->imgLabel->setPixmap(pixmap);

    // 上传图⽚，并删除图⽚
    uploadPhoto(firstFrame);
    QFile::remove(firstFrame);

    // 获取视频总时⻓，不需要显⽰播放窗⼝
    mpvPlayer = new MpvPlayer();
    mpvPlayer->startPlay(videoFilePath);
    mpvPlayer->pause();
    connect(mpvPlayer, &MpvPlayer::durationChanged, this, &UploadVideoPage::getDurationDone);
}

void UploadVideoPage::uploadPhoto(const QString &photoPath)
{
    // 读取图⽚数据，并将视频封⾯图上传服务器
    QByteArray fileData = loadFileToByteArray(photoPath);
    if(fileData.isEmpty())
    {
        LOG() << "视频封⾯图⽚失败";
        return;
    }

    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->uploadPhotoAsync(fileData, ui->imgLabel);


}

void UploadVideoPage::getDurationDone(int64_t duration)
{
    LOG() << duration;
    this->duration = duration;
    isDurationOk = true;
    delete mpvPlayer;
    mpvPlayer = nullptr;
}

