#ifndef MYSELFWIDGET_H
#define MYSELFWIDGET_H

#include <QWidget>

namespace Ui {
class MyselfWidget;
}

class MyselfWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyselfWidget(QWidget *parent = nullptr);
    ~MyselfWidget();

    // 获取⾃⼰信息
    void getMyselfInfo();

    // 加载当前⽤⼾信息
    void loadMyself();

    // 获取其他⽤⼾信息
    void getOtherUserInfo(const QString &otherUserId);

    // 加载其他⽤⼾信息
    void loadOtherUser(const QString& otherUserId);

private:
    // 初始化界⾯控件
    void initUI();

    // 点击上传头像按钮
    void uploadAvatarBtnClicked();

    // 点击设置按钮
    void settingBtnClicked();

    // 上传视频按钮槽函数
    void uploadViewBtnClicked();

    // 绑定信号槽
    void connectSignalAndSlots();

    // 是否隐藏界⾯元素：临时⽤⼾ 和 其他⽤⼾显⽰内容不同
    void hideWidget(bool isHide);

    // 获取⾃⼰的视频列表
    void getUserVideoList(const QString &userId, int pageIndex);

    // 清空视频列表
    void clearVideoList();

private slots:
    // 获取个⼈⽤⼾信息完成
    void getMyselfInfoDone();

    // 获取⽤⼾头像槽函数
    void getAvatarDone(const QString &fileId, const QByteArray &data);

    // 图⽚上传分两个阶段处理：① 上传图⽚⽂件 ② 修改图⽚
    void uploadAvatarDone1(const QString &fileId);
    void uploadAvatarDone2();

    // 获取视频列表成功信号
    void getUserVideoListDone(const QString& userId);

    // 当QSCrollArea滑动到底部时，需要获取下⼀⻚视频
    void onScrollAreaValueChanged(int value);

    // 删除视频
    void deleteVideo(const QString &videoId);

    // 删除视频成功信号槽函数
    void deleteVideoDone(const QString &videoId);

    // 获取其他⽤⼾信息成功后对应槽函数
    void getOtherUserInfoDone();

signals:
    // 切换到上传视频⻚⾯
    void switchUploadVideoPage(int pageIndex);



private:
    Ui::MyselfWidget *ui;

    QString userId; // 标记我的⻚⾯中显⽰的是当前⽤⼾还是其他⽤⼾
};

#endif // MYSELFWIDGET_H
