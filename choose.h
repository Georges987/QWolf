#ifndef CHOOSE_H
#define CHOOSE_H

#include <QDialog>
#include <QtWidgets>
#include "user.h"
#include "cliuser.h"

namespace Ui {
class Choose;
}

class Choose : public QDialog
{
    Q_OBJECT

public:
    explicit Choose(QString title = "Faite un choix", QString desc = "Veuillez faire un choix", QList<CliUser> utlisateur = QList<CliUser>(), QWidget *parent = nullptr);
    ~Choose();
    CliUser send_result();
    QList<CliUser> list_user;

private slots:
    void on_valid_box_accepted();

    void on_valid_box_rejected();

private:
    Ui::Choose *ui;
    CliUser user_choosed;
};

#endif // CHOOSE_H
