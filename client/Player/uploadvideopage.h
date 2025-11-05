#ifndef UPLOADVIDEOPAGE_H
#define UPLOADVIDEOPAGE_H

#include <QWidget>
#include "mpv/mpvplayer.h"

namespace Ui {
class UploadVideoPage;
}

class UploadVideoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UploadVideoPage(QWidget *parent = nullptr);
    ~UploadVideoPage();

    // 设置待上传⽂件名称到界⾯
    void setVideoTitle(const QString &videoFilePath);

private slots:
    // 提交按钮槽函数
    void onCommitBtnClicked();

    // 字数实时变化的两个函数
    // 标题QLineEdit::textChanged信号对应槽函数
    void onLineEditTextChanged(const QString &text);
    // 简介QPlainTextEdit::textChanged信号对应槽函数
    void onPlainEditTextChanged();

    // 更改视频封⾯图按钮槽函数
    void onChangeBtnClicked();

    // QComoBox中分类选择改变槽函数
    void onUpdataTags(const QString &kind);

    // 将kind下标签以按钮形式展⽰在界⾯上
    void addTagsByKind(const QString &kind);

    // 上传视频完成
    void onUploadVideoDone(const QString &videoId);

    // 上传封⾯图
    void uploadPhoto(const QString &photoPath);

    // 获取视频总时⻓成功
    void getDurationDone(int64_t duration);

    // 视频信息上传成功后，清空本次⻚⾯上留下的数据
    void resetPage();

signals:
    // 提交后，从当前页面切换回我的⻚⾯ 的信号
    void switchMyselfPage(int pageIndex);

private:
    Ui::UploadVideoPage *ui;

    // 上传视频路径
    QString videoFilePath;
    // 标记视频是否上传成功
    bool isUploadVideoOk = false;

    // 视频Id
    QString videoId;

    // 视频封⾯上传成功后返回视频封⾯id，上传视频信息时需⽤到
    QString coverImgId;
    // 标记图⽚是否上传成功
    bool isUploadPhotoOk = false;

    MpvPlayer* mpvPlayer;
    int64_t duration;
    bool isDurationOk = false;
};

#endif // UPLOADVIDEOPAGE_H
