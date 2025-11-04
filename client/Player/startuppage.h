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
    // 标记登录是否成功：临时⽤⼾登录 或 sessionId登录成功，都算登录成功
    bool loginSuccess = false;

    // 获取⽤⼾信息是否成功
    bool mySelfInfoSuccess = false;
};

#endif // STARTUPPAGE_H
