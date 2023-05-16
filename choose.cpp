#include "choose.h"
#include "ui_choose.h"

Choose::Choose(QString title, QString desc, QList<CliUser> utilisateur, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Choose)
{
    ui->setupUi(this);
    setWindowTitle(title);
    list_user = utilisateur;
    for (int i(0); i < utilisateur.length() ; i++ ) {
        ui->box->addItem(list_user[i].username);
    }
    ui->description_text->setText(desc);
}

Choose::~Choose()
{
    delete ui;
}

void Choose::on_valid_box_accepted()
{
    int val(0);
    for (int i(0); i < list_user.length() ; i++ ) {
        if(list_user[i].username.trimmed() == ui->box->currentText().trimmed())
            val = i;
    }
    user_choosed = list_user[val];
}

CliUser Choose::send_result()
{
    return user_choosed;
}

void Choose::on_valid_box_rejected()
{

}

