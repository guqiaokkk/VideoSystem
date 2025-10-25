#ifndef BARRAGEEDIT_H
#define BARRAGEEDIT_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class BarrageEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit BarrageEdit(QWidget *parent = nullptr);

signals:

private:
    QPushButton *sendBSBtn;
};

#endif // BARRAGEEDIT_H
