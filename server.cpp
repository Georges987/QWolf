#include "server.h"
#include "ui_server.h"
#include "validation.h"

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    setWindowTitle(tr("Wolf Server"));
    server_notif(tr("Initialisation du server ..."));

    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, 50887))
    {
        ui->server_console->append(tr("Le serveur n'a pas pu être démarré. \nRaison : ") + server->errorString());
    }
    else
    {
        // Si le serveur a été démarré correctement
        ui->server_console->append(tr("Le serveur a été démarré sur le port <strong>") + QString::number(server->serverPort()) + tr("</strong>.<br />Des clients peuvent maintenant se connecter."));
        connect(server, SIGNAL(newConnection()), this, SLOT(new_connection()));
    }
    tail = 0;
}

void Server::new_connection()
{
    QTcpSocket *nouveauClient = server->nextPendingConnection();

    utilisateurs << nouveauClient;

    User *new_user = new User(utilisateurs.indexOf(nouveauClient), "Wolfy " + QString::number( utilisateurs.length()));
    users << new_user;

    connect(nouveauClient, SIGNAL(readyRead()), this, SLOT(receive_data()));
    connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(disconnect_client()));

    server_notif(tr("Nouvel utilisateur"));
}

void Server::receive_data()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());

    if (socket == 0)
        return;

    QDataStream in(socket);

    if (users[utilisateurs.indexOf(socket)]->user_tail == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> users[utilisateurs.indexOf(socket)]->user_tail;
    }

    if (socket->bytesAvailable() <users[utilisateurs.indexOf(socket)]->user_tail)
        return;

    QString message;

    in >> message;

    send_message(message);
    users[utilisateurs.indexOf(socket)]->user_tail = 0;
}

void Server::disconnect_client()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());

    if(socket == 0)
        return;

    server_notif("<em>" + users[utilisateurs.indexOf(socket)]->name() + tr(" vient de se déconnecter</em>"));
    send_message("<em>" + users[utilisateurs.indexOf(socket)]->name() + tr(" vient de se déconnecter</em>"));

    users.removeAt(utilisateurs.indexOf(socket));

    utilisateurs.removeOne(socket);

    socket->deleteLater();
}

void Server::send_message(const QString &message)
{
    server_notif(tr("Envoi de donnée en cours ..."));

    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);

    out << (quint16) (paquet.size() - sizeof(quint16));

    for (int i = 0; i < utilisateurs.size(); i++)
    {
        utilisateurs[i]->write(paquet);
    }

    server_notif(tr("Données transférées avec succès"));
}

Server::~Server()
{
    delete ui;
    delete server;
}

void Server::on_quit_server_clicked()
{
    server->close();
    this->close();
}

void Server::on_stat_clicked()
{
    ui->server_console->clear();
    QString message = tr("*****************************************\n");
    message += tr("*Nombre d'utilisateurs en ligne : ") + QString::number(utilisateurs.length()) + "\n";
    message += tr("");
    message += tr("*****************************************");
    server_notif(message);
}

void Server::server_notif(const QString &message)
{
    ui->server_console->append(message);
}

void Server::on_clear_screen_clicked()
{
    ui->server_console->clear();
}


void Server::on_launch_server_clicked()
{
    Validation *valid = new Validation(true);
    valid->show();
}

void Server::on_list_users_clicked()
{
    server_notif("******************************");
    for (int i(0); i < utilisateurs.length() ; i++ ) {
        server_notif(QString::number(users[i]->index) + " | " + users[i]->name() + " -*- " + users[i]->card().name());
    }
    server_notif("******************************");
}

