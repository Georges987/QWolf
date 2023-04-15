#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainScreen; }
QT_END_NAMESPACE

class MainScreen : public QMainWindow
{
    Q_OBJECT

public:
    MainScreen(QWidget *parent = nullptr);
    ~MainScreen();

private slots:
    void on_create_server_clicked();

    void on_join_server_clicked();

    void on_quit_game_clicked();

private:
    Ui::MainScreen *ui;
};
#endif // MAINSCREEN_H
