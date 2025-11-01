#ifndef STARTUPPAGE_H
#define STARTUPPAGE_H

#include <QDialog>

class startupPage : public QDialog
{
    Q_OBJECT
public:
    explicit startupPage(QDialog *parent = nullptr);
    void startup();

private:
    // 标记临时⽤⼾是否登录成功
    bool loginSuccess = false;
};

#endif // STARTUPPAGE_H
