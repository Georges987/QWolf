#include "validation.h"
#include "ui_validation.h"
#include "chat.h"

Validation::Validation(bool isadmin, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Validation)
{
    ui->setupUi(this);
    ui->pseudo->setText("Wolfy");
    ui->IP->setText("127.0.0.1");
    admin_session = isadmin;
}

Validation::~Validation()
{
    delete ui;
}

void Validation::on_launch_game_clicked()
{
    Chat *chat_screen = new Chat(admin_session, ui->pseudo->text(), ui->IP->text());
    chat_screen->show();
    this->close();
}

void Validation::on_annuler_clicked()
{
    this->close();
}

