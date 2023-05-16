#ifndef SERVER_H
#define SERVER_H

#include "user.h"
#include "cliuser.h"

#include <QtWidgets>
#include <QtNetwork>

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    void send_message(const QString &message, int index = 0);
    void server_notif(const QString &message);
    QString check(const QString &message);
    bool verify_name(const QString &name);
    void restart();
    ~Server();

private slots:
    void new_connection();
    void disconnect_client();
    void receive_data();
    void start_game();
    void onTimerTick();

    void on_quit_server_clicked();

    void on_stat_clicked();

    void on_clear_screen_clicked();

    void on_launch_server_clicked();

    void on_list_users_clicked();

    void on_pushButton_clicked();

private:
    Ui::Server *ui;
    int count_loup;
    int count_kill;
    int count_chef;
    int count_tour;
    QTcpServer *server;
    QList<User *> users;
    QList<User *> wolf_list;
    bool verrou = false;
    bool game_started;
    QTimer *timer;
    QList<QTcpSocket *> utilisateurs;
    quint16 tail;
};

#endif // SERVER_H
