#ifndef PLAYERSERVER_H
#define PLAYERSERVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class PlayerServer;
}
QT_END_NAMESPACE

class PlayerServer : public QWidget
{
    Q_OBJECT

public:
    PlayerServer(QWidget *parent = nullptr);
    ~PlayerServer();

private:
    Ui::PlayerServer *ui;
};
#endif // PLAYERSERVER_H
