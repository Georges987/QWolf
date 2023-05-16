#include "server.h"
#include "ui_server.h"
#include "validation.h"


/**
 * @brief Server::Server
 * @param parent
 * - ajouter des cannaux de discussion
 */

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    setWindowTitle(tr("We're Wolf Server"));
    server_notif(tr("Initialisation du server ..."));
    timer = new QTimer();
    game_started = false;
    count_loup = 0;
    count_chef = 0;
    count_tour = 0;
    count_kill = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTick()));
    timer->setInterval(300000);
    //timer->setInterval(600000);

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
    server_notif(tr("Welcome to wolf server and good game. Please don't die. You're the admin"));
    tail = 0;
}

void Server::start_game()
{
    game_started = true;
    ui->pushButton->setDisabled(true);
    Card ca = Card("Villageois", "Simple villageois dont la mission est de trouver le loup. Bref c'est juste de la bonne bouffe pour le loup garou.");

    Card wolf = Card("Loup", "Seule contre tout le village, c'est soit lui qui bute tout le monde. Notice : c'est trop le pieds quand t'es le loup et en même temps le chef du village. Cela dit attirer les projecteur, ça a du bon et du mauvais. ");

    for (int i(0); i < users.length(); i++) {
        users[i]->set_card(ca);
    }

    server_notif("Starting game: assigning roles");
    if(utilisateurs.length() <= 4)
    {
        users[rand() % utilisateurs.length()]->set_card(wolf);
    }
    else if(utilisateurs.length() <= 6 && utilisateurs.length() >= 4)
    {
        int val1 = rand() % utilisateurs.length();
        int val2 = rand() % utilisateurs.length();

        if(val1 == val2)
        {
            val1 += 1;
            if(val1 >= users.length())
                val1 += -2;
        }

        users[val1]->set_card(wolf);
        users[val2]->set_card(wolf);

        wolf_list << users[val1];
        wolf_list << users[val2];
    }
    else if(utilisateurs.length() <= 10 && utilisateurs.length() >= 15)
    {
        int val1 = rand() % utilisateurs.length();
        int val2 = rand() % utilisateurs.length();
        int val3 = rand() % utilisateurs.length();

        if(val1 == val2 == val3 || val1 == val2 || val1 == val3 || val2 == val3)
        {
            val1 += 1;
            if(val1 >= users.length())
                val1 += -2;
        }

        users[val1]->set_card(wolf);
        users[val2]->set_card(wolf);
        users[val3]->set_card(wolf);

        wolf_list << users[val1];
        wolf_list << users[val2];
        wolf_list << users[val3];
    }

    QString message = ":cli2*|";

    for (int i(0); i < users.length() ; i++ ) {
        message += users[i]->card().name() + "///" + users[i]->card().description() + "|";
    }

    message += "*";

    for (int i(0); i <users.length() ;i++ ) {
        message += users[i]->name() + "///" + QString::number(users[i]->index) + "|";
    }

    send_message(message);
}

void Server::new_connection()
{
    QTcpSocket *nouveauClient = server->nextPendingConnection();
    if(utilisateurs.indexOf(nouveauClient) == -1)
    {

        utilisateurs << nouveauClient;

        User *new_user = new User(utilisateurs.indexOf(nouveauClient), "Wolfy " + QString::number( utilisateurs.length()));
        users << new_user;
        connect(nouveauClient, SIGNAL(readyRead()), this, SLOT(receive_data()));


        if(game_started)
        {
            send_message("Une partie est déjà en cours. Veuillez attendre la fin du jeu pour pouvoir vous connecter au serveur.", utilisateurs.indexOf(nouveauClient));
            nouveauClient->abort();
            utilisateurs.removeOne(nouveauClient);
            users.removeOne(new_user);
            return;
        }

        connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(disconnect_client()));

        server_notif("Nouvelle utilisateurs connecté Désormais " + QString::number(utilisateurs.length()) + "joueurs en ligne");


    }
    else
        QMessageBox::information(this, "Critical error detected", "Absolutely impossible case activated. Programm will stop. Please restart or contact Sys Admin (game developer)");
}

bool Server::verify_name(const QString &name)
{
    bool checker = false;

    for (int i(0); i<users.length() ;i++ ) {
        if(users[i]->name().trimmed() == name.trimmed())
            checker = true;
    }

    return checker;
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

    users[utilisateurs.indexOf(socket)]->user_tail = 0;

    if(check(message) == ":sys0")
    {
        server_notif("Assigation terminé pour " + users[utilisateurs.indexOf(socket)]->name());
    }
    else if(check(message) == ":sys1")
    {
        QStringList rest = message.split("|");
        if(!verify_name(rest[1]))
        {

            users[utilisateurs.indexOf(socket)]->set_name(rest[1]);
            if(rest[2].toInt() && !verrou)
            {
                users[utilisateurs.indexOf(socket)]->set_admin(true);
                verrou = true;
            }
            else
                users[utilisateurs.indexOf(socket)]->set_admin(false);

            send_message(":cli1|"+ users[utilisateurs.indexOf(socket)]->name()+ "|"+ QString::number(users[utilisateurs.indexOf(socket)]->index), utilisateurs.indexOf(socket));
        }
        else
        {
            QString msg = ":cli3|";
            for (int i(0); i < utilisateurs.length() ; i++ ) {
                msg += users[i]->name() + "///";
            }
            send_message(msg, utilisateurs.indexOf(socket));
        }
    }
    else if(check(message) == ":sys2")
    {
        QStringList rest = message.split("|");
        CliUser val = CliUser(rest[1].toInt(), rest[2]);
        server_notif("Chief vote. +1 for "+ rest[2]);
        users[rest[1].toInt()]->vote_chef += 1;
        count_chef++;

        if(count_chef >= utilisateurs.length())
        {
            int chef = 0;
            int max = users[0]->vote_chef;
            int compteur = 1;

            for (int i = 1; i < users.length(); ++i) {
                if (users[i]->vote_chef > max) {
                    max = users[i]->vote_chef;
                    chef = users[i]->index;
                    compteur = 1;
                }
                else if (users[i]->vote_chef == max) {
                    compteur++;
                    server_notif("Egalité");
                }
            }

            if(max <= 0) {
                server_notif("Nani the fuck.... Ils ont pas votés les connards ?. Comm");
                qApp->exit();
            }
            else{
                if(compteur != 1)
                {
                    send_message(":cli4|"+users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2]+" soit le chef. \nMais le vote à fini sur une égalité. L'admin devient alors le chef. Bonne chance pour vous débrouiller.\nVous pensiez quand même pas que le système allais se charger de faire la répartition. Mdr.");
                }
                else
                {
                    users[chef]->set_chief(true);
                    send_message(":cli5|"+users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2]+"Bref nouveau chef détecté. Butez ce connard c'est seulement moi votre cher master of game le chef de jeu ici. \nSon nom est " + users[chef]->name() + "|"+QString::number(users[chef]->index));
                }
            }
        }
        else
            send_message(users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2]+" soit le chef.");
    }
    else if(check(message) == ":sys3")
    {
        QStringList rest = message.split("|");
        CliUser val = CliUser(rest[1].toInt(), rest[2]);
        server_notif("Wolf vote. +1 for "+ rest[2]);
        users[rest[1].toInt()]->vote_loup += 1;
        count_loup++;

        if(count_loup >= utilisateurs.length())
        {
            int max = users[0]->vote_loup;
            int compteur = 1;
            for (int i = 1; i < users.length(); ++i) {
                if (users[i]->vote_loup > max) {
                    max = users[i]->vote_loup;
                    compteur = 1;
                }
                else if (users[i]->vote_loup == max) {
                    compteur++;
                }
            }

            if(max <= 0) {
                server_notif("Nani the fuck.... Ils ont pas votés les connards ?. Comment c'est possible. Bref vous vous foutez de moi. Je vais tous vous déconnecter. On rcommence");
                qApp->exit();
            }
            else{
                if(compteur != 1)
                {
                    send_message(":cli6|"+users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2]+" soit buté. \nMais le vote à fini sur une égalité. Le chef va donc choisir qui passe à l'échafaud.");
                }
                else
                {
                    users[max]->set_chief(true);
                    send_message(":cli7|"+users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2] +" meurt. Bref c'est donc " + users[max]->name() +"qui crève sur ce coup. Rip mec|"+QString::number(users[max]->index));
                }
            }
        }
        else
            send_message(users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2]+" soit tué.");
    }
    else if(check(message) == ":sys4")
    {
        QStringList rest = message.split("|");

        server_notif("Var it " + rest[2]);
        CliUser val = CliUser(rest[1].toInt(), rest[2]);
        server_notif("Admin vote. +1 for "+ rest[2]);
        send_message(":cli7|"+users[utilisateurs.indexOf(socket)]->name()+" qui est le chef a voté pour"+rest[2]+". Bref c'est donc " + users[rest[1].toInt()]->name() +"qui crève sur ce coup. Rip mec|"+QString::number(users[rest[1].toInt()]->index));
    }
    else if(check(message) == ":sys5")
    {
        QStringList rest = message.split("|");
        CliUser val = CliUser(rest[1].toInt(), rest[2]);
        server_notif("Wolf kill vote. +1 for "+ rest[2]);
        users[rest[1].toInt()]->vote_kill += 1;
        count_kill++;

        if(count_kill >= wolf_list.length())
        {
            int max = users[0]->vote_kill;
            int compteur = 1;
            for (int i = 1; i < wolf_list.length(); ++i) {
                if (users[i]->vote_kill > max) {
                    max = users[i]->vote_kill;
                    compteur = 1;
                }
                else if (users[i]->vote_kill == max) {
                    compteur++;
                }
            }

            if(max <= 0) {
                server_notif("Nani the fuck.... Ils ont pas votés les connards ?. Comment c'est possible. Bref vous vous foutez de moi. Je vais tous vous déconnecter. On recommence");
                qApp->exit();
            }
            else{
                if(compteur != 1)
                {
                    send_message(":cli6|"+users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2]+" soit buté. \nMais le vote à fini sur une égalité. Le chef va donc choisir qui passe à l'échafaud.");
                }
                else
                {
                    users[max]->set_chief(true);
                    send_message(":cli7|"+users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2] +" meurt. Bref c'est donc " + users[max]->name() +"qui crève sur ce coup. Rip mec|"+QString::number(users[max]->index));
                }
            }
        }
        else
            send_message(users[utilisateurs.indexOf(socket)]->name()+" a voté pour que "+rest[2]+" soit tué.");
    }
    else
        send_message(message);
}

QString Server::check(const QString &message)
{
    QString checker;
    checker = message[0];
    checker += message[1];
    checker += message[2];
    checker += message[3];
    checker += message[4];
    return checker;
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
    if(utilisateurs.length() == 0)
        ui->launch_server->setDisabled(false);
}

void Server::send_message(const QString &message, int index)
{
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);

    out << (quint16) (paquet.size() - sizeof(quint16));

    if(index == 0)
    {
        for (int i = 0; i < utilisateurs.size(); i++)
        {
            utilisateurs[i]->write(paquet);
            utilisateurs[i]->flush();
        }
    }
    else
    {
        utilisateurs[index]->write(paquet);
        server_notif("User specific Message");
        utilisateurs[index]->flush();
    }
}

Server::~Server()
{
    delete ui;
    delete timer;
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
    QString message = tr("*****************************************\n*Nombre d'utilisateurs en ligne : ") + QString::number(utilisateurs.length()) + tr("\n*****************************************");
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
    ui->launch_server->setDisabled(true);
}

void Server::on_list_users_clicked()
{
    ui->server_console->clear();
    server_notif("******************************");
    if(!users.isEmpty())
    {
        for (int i(0); i < utilisateurs.length() ; i++ ) {
            server_notif(QString::number(users[i]->index) + " | " + users[i]->name() + " -*- " + users[i]->card().name());
        }
    }
    else
        server_notif("No user connected");
    server_notif("******************************");
}


void Server::on_pushButton_clicked()
{
    if(utilisateurs.length() >= 3)
        start_game();
    else
        server_notif("\nNombre de joueurs inférieurs à 3. Veuillez attendre l'arrivée de nouveau joueur pour pouvoir lancer le joueur.");
}

void Server::onTimerTick()
{
    server_notif("Timer tick");
    restart();
}

void Server::restart()
{
    server_notif("Ah mon horloge interne crie famine");
}
