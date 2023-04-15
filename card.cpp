#include "card.h"

Card::Card(QString namae, QString desc)
{
    card_name = namae;
    card_description = desc;
}

QString Card::description()
{
    return card_description;
}

QString Card::name()
{
    return card_name;
}

void Card::set_name(QString namae)
{
    card_name = namae;
}

void Card::set_description(QString desc)
{
    card_description = desc;
}
