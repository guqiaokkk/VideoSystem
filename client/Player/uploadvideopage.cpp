#include "uploadvideopage.h"
#include "ui_uploadvideopage.h"

#include "util.h"

#include <QFileDialog>

UploadVideoPage::UploadVideoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UploadVideoPage)
{
    ui->setupUi(this);

    // 设置标签不拦截鼠标事件
    ui->imgLabel->setAttribute(Qt::WA_TransparentForMouseEvents);




    // 提交按钮
    connect(ui->commitBtn, &QPushButton::clicked, this, &UploadVideoPage::onCommitBtnClicked);

    // LineEdit和PlainTextEdit中内容变化，就会发射textChanged信号
    connect(ui->videoTitle, &QLineEdit::textChanged, this, &UploadVideoPage::onLineEditTextChanged);
    connect(ui->plainTextEdit, &QPlainTextEdit::textChanged, this, &UploadVideoPage::onPlainEditTextChanged);

    connect(ui->changeButton, &QPushButton::clicked, this, &UploadVideoPage::onChangeBtnClicked);
}

UploadVideoPage::~UploadVideoPage()
{
    delete ui;
}

void UploadVideoPage::onCommitBtnClicked()
{
    // 视频上传成功，切换到我的⻚⾯
    emit switchMyselfPage(1);
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
}
