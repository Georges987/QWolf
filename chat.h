#ifndef CHAT_H
#define CHAT_H

#include "user.h"
#include <QMainWindow>
#include <QtWidgets>
#include <QtNetwork>

namespace Ui {
class Chat;
}

class Chat : public QMainWindow
{
    Q_OBJECT

public:
    explicit Chat(bool isadmin = false, QString pseudo = "Undefined", QString addr = "127.0.0.1", QWidget *parent = nullptr);
    ~Chat();
private slots:
    void receive_data();
    void connecte();
    void deconnecte();
    void erreurSocket(QAbstractSocket::SocketError erreur);
    void send_text(const QString &message);
    void treat_sys(const QString &message);
    void on_chat_sent_btn_clicked();

    void on_reload_co_clicked();

    void on_disonnection_clicked();

private:
    Ui::Chat *ui;
    User *me;
    QString *host_adr;
    QTcpSocket *server; // Représente le serveur
};

#endif // CHAT_H
