#include "edituserdialog.h"
#include "ui_edituserdialog.h"

#include "player.h"

EditUserDialog::EditUserDialog(QWidget *parent, const QString& text)
    : QDialog(parent)
    , ui(new Ui::EditUserDialog)
{
    ui->setupUi(this);

    // 去除窗⼝边框
    setWindowFlag(Qt::FramelessWindowHint);
    // 设置窗⼝透明
    setAttribute(Qt::WA_TranslucentBackground);
    // 设置窗⼝描述
    ui->titleLabel->setText(text);

    // 将窗⼝位置移动到和主窗⼝左上⻆重合
    Player *player = Player::getInstance();
    this->move(player->mapToGlobal(QPoint(0, 0)));

    // 取消按钮点击
    connect(ui->cancelBtn, &QPushButton::clicked, this, &EditUserDialog::onCancelBtnClicked);


    // 对⼿机号编辑框需进⾏限制：以1开始的11位整形数字
    QRegularExpression regExp("^1\\d{10}$");
    QValidator* validator = new QRegularExpressionValidator(regExp, this);
    ui->phoneEdit->setValidator(validator);

    // 设置⽤⼾⻆⾊
    ui->roleComboBox->addItem("平台管理员");
    ui->roleComboBox->setCurrentIndex(0);

    // 编辑框内容变化信号
    connect(ui->commentTextEdit, &QPlainTextEdit::textChanged, this, [=](){
        QString text = ui->commentTextEdit->toPlainText();
        int wordCount = text.size();
        if(wordCount <= 10){
            ui->wordCount->setText(QString::number(wordCount) + "/10");
        }else{
            ui->commentTextEdit->setPlaceholderText(text.mid(0,10));
        }
    });
}

EditUserDialog::~EditUserDialog()
{
    delete ui;
}

void EditUserDialog::onCancelBtnClicked()
{
    close();
}
