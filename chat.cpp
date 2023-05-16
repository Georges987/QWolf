#include "chat.h"
#include "validation.h"
#include "ui_chat.h"
#include "choose.h"

Chat::Chat(bool isadmin, QString pseudo, QString addr, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    me = new User();
    ui->choose_chief->setHidden(true);
    ui->choose_wolf->setHidden(true);

    timer = new QTimer(this);
    timer->setInterval(30000);

    connect(timer, SIGNAL(timeout()), this, SLOT(end_night()));

    if(isadmin)
        ui->pseudo->setText("Administrateur : " + pseudo);
    else
        ui->pseudo->setText("Joueur : " + pseudo);
    ui->choose_wolf->setDisabled(false);
    ui->chat_input->setFocus();
    me->set_admin(isadmin);
    me->set_name(pseudo);
    host_adr = QString(addr);
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

    if(treat_sys(messageRecu) == ":cli1")
    {
        QStringList rest = messageRecu.split("|");
        me->index = rest[2].toInt();
    }
    else if(treat_sys(messageRecu) == ":cli2")
    {

        ui->choose_chief->setHidden(false);
        ui->choose_wolf->setHidden(false);

        QStringList rest = messageRecu.split("*");

        QStringList roles = rest[1].split("|");
        QStringList myval = roles[me->index + 1].split("///");
        Card ca = Card(myval[0], myval[1]);
        me->set_card(ca);

        ui->chat_screen->clear();
        ui->chat_screen->append("System message : Votre carte est " + me->card().name());
        ui->chat_screen->append("Description : " + me->card().description());

        ui->disonnection->hide();
        ui->line->hide();

        QStringList users = rest[2].split("|");

        for (int i(0); i < users.length() - 1 ; i++) {
            QStringList temp = users[i].split("///");
            if(temp[1].toInt() == me->index)
                continue;
            else
            {
                CliUser ua = CliUser(temp[1].toInt(), temp[0]);
                list_user.append(ua);
            }
        }

        ui->chat_screen->append("System message : List des joueurs :\n");

        for (int i(0); i < list_user.length() ; i++) {
            ui->chat_screen->append(QString::number(list_user[i].identifier) + list_user[i].username);
        }

        ui->chat_screen->append("Qui est le loup parmi vous ?");

    }
    else if(treat_sys(messageRecu) == ":cli3")
    {
        ui->reload_co->setHidden(false);
        QStringList rest = messageRecu.split("|");
        ui->chat_screen->append("Déconnecté en raison d'un mauvais choix de nom d'utilisateur.\nVeuillez choisir un nom d'utilisateur qui ne fait pas partie de cette liste. \n");
        QStringList use = rest[1].split("///");
        for (int i(0); i < use.length() - 2;i++ ) {
            ui->chat_screen->append("\t - " + use[i]);
        }
        ui->chat_screen->append("\nPour relancer le jeu veuillez recharger la connection avec le bouton \"Recharger la connection\".");
        server->abort();
    }

    else if(treat_sys(messageRecu) == ":cli4")
    {
        QStringList rest = messageRecu.split("|");
        if(me->isadmin())
        {
            me->set_chief(true);
            ui->chat_screen->append("System message : Vous êtes désormais le chef de cette partie");
        }

        ui->chat_screen->append(rest[1]);
    }

    else if(treat_sys(messageRecu) == ":cli5")
    {
        QStringList rest = messageRecu.split("|");

        if(rest[2].toInt() == me->index)
        {
            me->set_chief(true);
            ui->chat_screen->append("System message : Vous êtes désormais le chef de cette partie");
        }
        ui->chat_screen->append(rest[1]);
    }

    else if(treat_sys(messageRecu) == ":cli6")
    {
        QStringList rest = messageRecu.split("|");
        ui->chat_screen->append(rest[1]);

        if(me->ischief() || me->isadmin())
        {
            CliUser val;
            Choose *ch_boss = new Choose("Choix du loup", "Veuillez faire le choix du loup. Faite bien votre choix. Le sort du village dépend de vous", list_user, this);
            ch_boss->exec();
            if(ch_boss->Accepted)
                val = ch_boss->send_result();
            ch_boss->close();
            for (int i(0);i < list_user.length() ;i++ ) {
                if(val.username == list_user[i].username)
                    val = list_user[i];
            }
            send_text(":sys4|"+ QString::number(val.identifier) +"|"+val.username);
        }
    }

    else if(treat_sys(messageRecu) == ":cli7")
    {
        QStringList rest = messageRecu.split("|");
        if(rest[2].toInt() == me->index)
        {
            mort();
            ui->chat_screen->append(rest[1]);
            ui->chat_screen->append("Vous êtes mort\n");
            send_text("Mon testament : \n"+me->card().name() + "\n"+me-> card().description());
        }
        timer->start();

        ui->chat_input->setDisabled(true);
        ui->chat_sent_btn->setDisabled(true);
        ui->choose_chief->setDisabled(true);
        ui->auto_kill->setDisabled(true);

        ui->chat_screen->append("La nuit tombe.");
        ui->chat_screen->append("Le ou les loups vont désormais choisir leur cibles");

        ui->chat_screen->append("Il restent 30 s");
        if(me->card().name().toLower().trimmed() == "wolf")
            ui->wolf_kill->setDisabled(false);


    }

    else
        ui->chat_screen->append(messageRecu);

    me->user_tail = 0;
}

QString Chat::treat_sys(const QString &message)
{
    QString checker;
    checker = message[0];
    checker += message[1];
    checker += message[2];
    checker += message[3];
    checker += message[4];
    return checker;
}

void Chat::connecte()
{
    ui->chat_screen->append(tr("<em>Connexion réussie !</em>"));
    int ad;
    if(me->isadmin())
        ad = 1;
    else
        ad = 0;
    send_text(tr(":sys1|") + me->name() + "|" + QString::number(ad));
    me->set_isalive(true);
    ui->reload_co->setHidden(true);
    connected = true;
}

void Chat::deconnecte()
{
    ui->chat_screen->append(tr("Déconnecté du serveur"));
    connected = false;
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
    if(!ui->chat_input->text().remove(QRegularExpression("\\s+")).isEmpty())
        send_text(me->name() + " : " + ui->chat_input->text());
    else
    {
        ui->chat_screen->clear();
        ui->chat_screen->append("*********\nVeuillez entrer du text avant de l'envoyer\n*********");
    }
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
    connected = false;
    this->close();
}

void Chat::on_disonnection_clicked()
{
    server->abort();
    this->close();
}

void Chat::on_choose_chief_clicked()
{
    CliUser val;
    Choose *ch_boss = new Choose("Choix du chef", "Veuillez faire le choix d'un chef pour cette partie de jeu. Il sera chargé de vous départager en cas d'égalité", list_user, this);
    ch_boss->exec();
    if(ch_boss->Accepted)
        val = ch_boss->send_result();
    ch_boss->close();
    for (int i(0);i < list_user.length() ;i++ ) {
        if(val.username == list_user[i].username)
            val = list_user[i];
    }
    if(val.username.toLower() != "undefined")
    {
        send_text(":sys2|"+ QString::number(val.identifier) +"|"+val.username);
        ui->choose_chief->setDisabled(true);
    }
    else
        return;
}

void Chat::on_see_card_clicked()
{
    ui->chat_screen->append("Nom de votre carte : " + me->card().name());
    ui->chat_screen->append("Description de votre carte : " + me->card().description());
}

void Chat::on_auto_kill_clicked()
{
    mort();
    send_text("Message system : le joueur " + me->name()+ " s'est suicidé. Le pauvre il avait trop peur. Souhaitons lui bonne chance. Les gens qui se suicident ne vont pas au paradis. Rip.");
}

void Chat::mort()
{
    me->set_isalive(false);
    ui->chat_input->setDisabled(true);
    ui->chat_sent_btn->setDisabled(true);
    ui->choose_chief->setDisabled(true);
    ui->auto_kill->setDisabled(true);
    ui->disonnection->setHidden(false);
}

void Chat::on_choose_wolf_clicked()
{
    CliUser val;
    Choose *ch_wolf = new Choose("Choix du loup", "Veuillez faire le choix du loup. Protégez votre village.", list_user, this);
    ch_wolf->exec();
    if(ch_wolf->Accepted)
        val = ch_wolf->send_result();
    ch_wolf->close();
    for (int i(0);i < list_user.length() ;i++ ) {
        if(val.username == list_user[i].username)
            val = list_user[i];
    }
    if(val.username.toLower() != "undefined")
    {
        send_text(":sys3|"+ QString::number(val.identifier) +"|"+val.username);
        ui->choose_wolf->setDisabled(true);
    }
    else
        return;
}


void Chat::on_wolf_kill_clicked()
{
    CliUser val;
    Choose *ch_wolf = new Choose("Choisir une victime", "Vous ne pouvez pas vous en empêcher. 'un des joueurs vas tousimplement y passer. Désignez votre nouvelle victime.", list_user, this);
    ch_wolf->exec();
    if(ch_wolf->Accepted)
        val = ch_wolf->send_result();
    ch_wolf->close();
    for (int i(0);i < list_user.length() ;i++ ) {
        if(val.username == list_user[i].username)
            val = list_user[i];
    }
    if(val.username.toLower() != "undefined")
    {
        send_text(":sys5|"+ QString::number(val.identifier) +"|"+val.username);
        ui->choose_wolf->setDisabled(true);
    }
    else
        return;
}

void Chat::end_night()
{
    if(me->isalive())
    {
        ui->chat_input->setDisabled(false);
        ui->chat_sent_btn->setDisabled(false);
        ui->choose_chief->setDisabled(false);
        ui->auto_kill->setDisabled(false);
    }
    timer->stop();
    ui->chat_screen->append("Fin de la selection. Le jour se lève.");
}
