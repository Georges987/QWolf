#include "mainscreen.h"
#include "server.h"
#include "validation.h"
#include "ui_mainscreen.h"

MainScreen::MainScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainScreen)
{
    ui->setupUi(this);
}

MainScreen::~MainScreen()
{
    delete ui;
}


void MainScreen::on_create_server_clicked()
{
    Server *serve_screen = new Server();
    serve_screen->show();
    //this->close();
}

void MainScreen::on_join_server_clicked()
{
    Validation *valid = new Validation(false);
    valid->show();
    //this->close();
}

void MainScreen::on_quit_game_clicked()
{
    qApp->exit();
}

