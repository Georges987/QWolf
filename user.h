#ifndef USER_H
#define USER_H

#include "card.h"
#include <QtNetwork>

class User
{
public:
    explicit User(int id = 0, QString name = "Undefined", bool isadministrateur = false, bool ischef = false, bool vivant = true, Card carte = Card());

    //setters
    void set_name(QString name);
    void set_admin(bool admin);
    void set_chief(bool chief);
    void set_card(Card card);
    void set_isalive(bool alive);

    //getters
    QString name();
    bool isadmin();
    bool ischief();
    bool isalive();
    Card card();
    quint16 user_tail;
    int index;

private:
    QString user_name;
    bool user_isadmin;
    bool user_ischief;
    bool user_isalive;
    Card user_card;
};

#endif // USER_H
