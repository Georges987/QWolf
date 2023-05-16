#ifndef CHAT_H
#define CHAT_H

#include "user.h"
#include <QMainWindow>
#include <QtWidgets>
#include <QtNetwork>
#include "cliuser.h"

namespace Ui {
class Chat;
}

class Chat : public QMainWindow
{
    Q_OBJECT

public:
    explicit Chat(bool isadmin = false, QString pseudo = "Wolfy", QString addr = "127.0.0.1", QWidget *parent = nullptr);
    ~Chat();
    void mort();
    void send_text(const QString &message);
    QString treat_sys(const QString &message);

private slots:
    void receive_data();
    void connecte();
    void deconnecte();
    void erreurSocket(QAbstractSocket::SocketError erreur);

    void on_chat_sent_btn_clicked();

    void on_reload_co_clicked();

    void on_disonnection_clicked();

    void on_choose_chief_clicked();

    void on_see_card_clicked();

    void on_auto_kill_clicked();


    void on_choose_wolf_clicked();

    void on_wolf_kill_clicked();

    void end_night();

private:
    Ui::Chat *ui;
    User *me;
    QList<CliUser> list_user;
    QString host_adr;
    bool connected = false;
    QTcpSocket *server; // Représente le serveur
    QTimer *timer;
};

#endif // CHAT_H
