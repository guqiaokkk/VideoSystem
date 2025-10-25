#ifndef UPLOADVIDEOPAGE_H
#define UPLOADVIDEOPAGE_H

#include <QWidget>

namespace Ui {
class UploadVideoPage;
}

class UploadVideoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UploadVideoPage(QWidget *parent = nullptr);
    ~UploadVideoPage();

private:
    // 提交按钮槽函数
    void onCommitBtnClicked();

    // 字数实时变化的两个函数
    // 标题QLineEdit::textChanged信号对应槽函数
    void onLineEditTextChanged(const QString &text);
    // 简介QPlainTextEdit::textChanged信号对应槽函数
    void onPlainEditTextChanged();

    // 更改视频封⾯图按钮槽函数
    void onChangeBtnClicked();

signals:
    // 提交后，从当前页面切换回我的⻚⾯ 的信号
    void switchMyselfPage(int pageIndex);

private:
    Ui::UploadVideoPage *ui;
};

#endif // UPLOADVIDEOPAGE_H
