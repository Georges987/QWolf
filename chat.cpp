#include "chat.h"
#include "validation.h"
#include "ui_chat.h"

Chat::Chat(bool isadmin, QString pseudo, QString addr, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    me = new User();
    ui->chat_input->setFocus();
    me->set_admin(isadmin);
    me->set_name(pseudo);
    host_adr = new QString(addr);
    server = new QTcpSocket(this);
    connect(server, SIGNAL(readyRead()), this,SLOT(receive_data()));
    connect(server, SIGNAL(connected()), this, SLOT(connecte()));
    connect(server, SIGNAL(disconnected()), this, SLOT(deconnecte()));
    connect(server, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(erreurSocket(QAbstractSocket::SocketError)));

    ui->chat_screen->append(tr("<em>Tentative de connexion en cours...</em>"));
    server->abort();
    server->connectToHost(addr, 50887);
    me->user_tail = 0;
}

Chat::~Chat()
{
    delete ui;
    delete me;
}

void Chat::receive_data()
{
    QDataStream in(server);
    if (me->user_tail == 0)
    {
        if (server->bytesAvailable() < (int)sizeof(quint16))
            return;
    }
    in >> me->user_tail;
    if (server->bytesAvailable() < me->user_tail)
        return;

    QString messageRecu;
    in >> messageRecu;

    me->user_tail = 0;

    QString check;
    check = messageRecu[0];
    check += messageRecu[1];
    check += messageRecu[2];
    check += messageRecu[3];
    check += messageRecu[4];

    if(check == ":sys1")
        treat_sys(messageRecu);
    else
        ui->chat_screen->append(messageRecu);

}

void Chat::treat_sys(const QString &message)
{
    QMessageBox::information(this, "Te", message);
}

void Chat::connecte()
{
    ui->chat_screen->append(tr("<em>Connexion réussie !</em>"));
}

void Chat::deconnecte()
{
    ui->chat_screen->append(tr("Déconnecté du serveur"));
}

void Chat::erreurSocket(QAbstractSocket::SocketError erreur)
{
    switch(erreur)
    {
    case QAbstractSocket::HostNotFoundError:
        ui->chat_screen->append(tr("<em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.</em>"));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        ui->chat_screen->append(tr("<em>ERREUR : le serveur a refusé la connexion. Vérifiez si le programme \"serveur\" a bien été lancé. Vérifiez aussi l'IP et le port.</em>"));
        break;
    case QAbstractSocket::RemoteHostClosedError:
        ui->chat_screen->append(tr("<em>ERREUR : le serveur a coupé la connexion.</em>"));
        break;
    default:
        ui->chat_screen->append(tr("<em>ERREUR : ") + server->errorString() + tr("</em>"));
    }
}

void Chat::on_chat_sent_btn_clicked()
{
    send_text(me->name() + " : " + ui->chat_input->text());
    ui->chat_input->clear();
    ui->chat_input->setFocus();
}

void Chat::send_text(const QString &message)
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    server->write(paquet); //envoi du paquet

}

void Chat::on_reload_co_clicked()
{
    Validation *valid = new Validation(me->isadmin());
    valid->show();
    server->abort();
    this->close();
}


void Chat::on_disonnection_clicked()
{
    server->abort();
    this->close();
}

