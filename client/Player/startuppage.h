#ifndef STARTUPPAGE_H
#define STARTUPPAGE_H

#include <QDialog>

class startupPage : public QDialog
{
    Q_OBJECT
public:
    explicit startupPage(QDialog *parent = nullptr);
    void startup();
};

#endif // STARTUPPAGE_H
