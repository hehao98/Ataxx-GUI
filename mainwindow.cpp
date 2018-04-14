#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"
#include "ataxx.h"
#include "ui_mainwindow.h"
#include "configdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->currPlayerNameLabel->setText(ui->player1NameLabel->text());

    connect(ui->actionNewGame, SIGNAL(triggered(bool)), this, SLOT(startNewGame()));
    connect(ui->actionSaveGame, SIGNAL(triggered(bool)), this, SLOT(saveGame()));
    connect(ui->actionLoadLastSave, SIGNAL(triggered(bool)), this, SLOT(loadLastSave()));
    connect(ui->actionPlayer_1, SIGNAL(triggered(bool)), this, SLOT(generateHintForPlayer1()));
    connect(ui->actionPlayer_2, SIGNAL(triggered(bool)), this, SLOT(generateHintForPlayer2()));
    connect(ui->actionConfig, SIGNAL(triggered(bool)), this, SLOT(popConfigDialog()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(popAboutMessage()));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(quitGame()));
    connect(ui->newGameButton, SIGNAL(clicked(bool)), this, SLOT(startNewGame()));
    connect(ui->saveGameButton, SIGNAL(clicked(bool)), this, SLOT(saveGame()));
    connect(ui->loadGameButton, SIGNAL(clicked(bool)), this, SLOT(loadLastSave()));
    connect(ui->configButton, SIGNAL(clicked(bool)), this, SLOT(popConfigDialog()));
    connect(ui->ataxxWidget, SIGNAL(statusChanged()), this, SLOT(refreshUI()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startNewGame()
{
    ui->ataxxWidget->initGrid(Ataxx::Player1);
    refreshUI();
}

void MainWindow::loadLastSave()
{
    ui->ataxxWidget->load();
    refreshUI();
    ui->ataxxWidget->setGridInterface();
}

void MainWindow::saveGame()
{
    ui->ataxxWidget->save();
}

void MainWindow::generateHintForPlayer1()
{
    Ataxx::Move m = ui->ataxxWidget->findNextMove(Ataxx::Player1, Ataxx::Insane);
    ui->ataxxWidget->showHint(m);
}

void MainWindow::generateHintForPlayer2()
{
    Ataxx::Move m = ui->ataxxWidget->findNextMove(Ataxx::Player2, Ataxx::Insane);
    ui->ataxxWidget->showHint(m);
}

void MainWindow::popConfigDialog()
{
    ConfigDialog configDialog(this);
    if (configDialog.exec() == QDialog::Rejected) {
        return;
    }
    ui->player1NameLabel->setText(configDialog.getNewPlayer1Name());
    ui->player2NameLabel->setText(configDialog.getNewPlayer2Name());
    ui->ataxxWidget->setPlayer1IsAI(configDialog.getPlayer1IsAI());
    ui->ataxxWidget->setPlayer2IsAI(configDialog.getPlayer2IsAI());
    ui->ataxxWidget->setPlayer1AIDifficulty(configDialog.getPlayer1AIDifficulty());
    ui->ataxxWidget->setPlayer2AIDifficulty(configDialog.getPlayer2AIDifficulty());
}

void MainWindow::popAboutMessage()
{
    QMessageBox::information(this, "About", "Created by 何昊");
}

void MainWindow::quitGame()
{
    QApplication::exit();
}

void MainWindow::refreshUI()
{
    if (ui->ataxxWidget->getCurrPlayer() == Ataxx::Player1) {
        ui->currPlayerNameLabel->setText(ui->player1NameLabel->text());
    } else {
        ui->currPlayerNameLabel->setText(ui->player2NameLabel->text());
    }
    ui->player1ScoreValue->setText(QString::number(ui->ataxxWidget->getScore(Ataxx::Player1)));
    ui->player2ScoreValue->setText(QString::number(ui->ataxxWidget->getScore(Ataxx::Player2)));
}

void MainWindow::handleGameOver(Ataxx::Player player)
{
    QString message;
    if (player == Ataxx::Player1) {
        message = "The black side wins!";
    } else {
        message = "The white side wins!";
    }
    QMessageBox::information(this, "Info", message);
}
