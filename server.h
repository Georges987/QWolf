#ifndef SERVER_H
#define SERVER_H

#include "user.h"

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
    void send_message(const QString &message);
    void server_notif(const QString &message);
    ~Server();

private slots:
    void new_connection();
    void disconnect_client();
    void receive_data();

    void on_quit_server_clicked();

    void on_stat_clicked();

    void on_clear_screen_clicked();

    void on_launch_server_clicked();

    void on_list_users_clicked();

private:
    Ui::Server *ui;
    QTcpServer *server;
    QList<User *> users;
    QList<QTcpSocket *> utilisateurs;
    quint16 tail;
};

#endif // SERVER_H
