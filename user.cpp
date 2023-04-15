#include "user.h"

User::User(int id, QString name, bool isadministrateur, bool ischef, bool vivant, Card carte)
{
    user_name = name;
    user_isadmin = isadministrateur;
    user_ischief = ischef;
    user_isalive = vivant;
    user_card = carte;
    index = id;
    user_tail = 0;
}

void User::set_name(QString name)
{
    user_name = name;
}

void User::set_isalive(bool alive)
{
    user_isalive = alive;
}
void User::set_admin(bool admin)
{
    user_isadmin = admin;
}

void User::set_chief(bool chief)
{
    user_ischief = chief;
}

void User::set_card(Card card)
{
    user_card = card;
}

//getters

QString User::name()
{
    return user_name;
}

bool User::isadmin()
{
    return user_isadmin;
}

bool User::ischief()
{
    return user_ischief;
}

bool User::isalive()
{
    return user_isalive;
}

Card User::card()
{
    return user_card;
}
