#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QList>

namespace Ui {
class HomePageWidget;
}

// 向服务器获取视频列表的⽅式
enum VideoListStyle
{
    AllStyle,   // 所有视频列表
    KindStyle,  // 分类视频列表
    TagStyle,   // 标签视频列表
    SearchStyle // 搜索视频列表
};


class HomePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomePageWidget(QWidget *parent = nullptr);
    ~HomePageWidget();

private:
    // 初始化分类和标签处的按钮
    void initKindsAndTags();
    // 绑定所有的信号槽
    void connectSignalAndSlot();

    // 创建按钮函数
    QPushButton *buildSelectBtn(QWidget *parent, const QString &color, const QString &text);
    // 当点击分类后，需要显⽰该分类下的标签
    void resetTags(const QList<QString> &tags);

    // 初始化刷新和置顶按钮
    void initRefreshAndTop();

    // 初始化视频列表, 从服务器获取视频所有视频列表
    void initVideos();

protected:
    // 分类按钮点击
    void onKindBtnClicked(QPushButton *clickedkindBtn);

    // 标签按钮点击
    void onTagBtnClicked(QPushButton *clickedtagBtn);

    // 刷新按钮点击槽函数
    void onRefreshBtnClicked();
    // 置顶按钮点击槽函数
    void onTopBtnClicked();

    // 更新视频列表
    void upDataVideoList();

    // 清空视频列表
    void clearLayoutVideos();

    // 搜索视频
    void onSearchVideos(const QString &searchText);

    // QScrollArea滚动条位置检测
    void onScrollAreaValueChanged(int value);

private:
    Ui::HomePageWidget *ui;

    // 记录当前分类和标签
    QString curKind;
    QString curTag;

    // 标记当前获取视频列表的⽅式
    VideoListStyle videoListStyle;
};

#endif // HOMEPAGEWIDGET_H
