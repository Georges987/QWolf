#ifndef CARD_H
#define CARD_H

#include <QtNetwork>

class Card
{
public:
    explicit Card(QString namae = "Undefined", QString desc = "Undefined");
    void set_name(QString namae);
    void set_description(QString desc);
    QString name();
    QString description();
private:
    QString card_name;
    QString card_description;
};

#endif // CARD_H
