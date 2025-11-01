#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QWidget>
#include <QLineEdit>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchLineEdit(QWidget *parent = nullptr);

signals:
    // 发送搜索信号给⾸⻚
    void searchVideos(const QString &searchText);

private:
    void searchBtnClicked();
};

#endif // SEARCHLINEEDIT_H
