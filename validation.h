#ifndef VALIDATION_H
#define VALIDATION_H

#include <QWidget>

namespace Ui {
class Validation;
}

class Validation : public QWidget
{
    Q_OBJECT

public:
    explicit Validation(bool idadmin = false, QWidget *parent = nullptr);
    ~Validation();

private slots:
    void on_launch_game_clicked();

    void on_annuler_clicked();

private:
    Ui::Validation *ui;
    bool admin_session;
};

#endif // VALIDATION_H
