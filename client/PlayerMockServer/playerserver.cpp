#include "playerserver.h"
#include "./ui_playerserver.h"

PlayerServer::PlayerServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerServer)
{
    ui->setupUi(this);
}

PlayerServer::~PlayerServer()
{
    delete ui;
}
