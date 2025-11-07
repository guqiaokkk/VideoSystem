#ifndef CONFIRMDIALOG_H
#define CONFIRMDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLabel>

class ConfirmDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfirmDialog(QWidget *parent = nullptr);

    // 让MySelfWidget获取⽤⼾是否真要退出登录
    bool getConfirm()const;

    // 设置界⾯中的显⽰⽂本
    void setText(const QString &text);

signals:

private:
    bool isConfirm = false;
    QLabel *textLabel;
};

#endif // CONFIRMDIALOG_H
