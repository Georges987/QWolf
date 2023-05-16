#ifndef CLIUSER_H
#define CLIUSER_H

#include <QWidget>

class CliUser
{
public:
    CliUser(int id = 0, QString name = "Undefined");
    QString username;
    int identifier;
};

#endif // CLIUSER_H
