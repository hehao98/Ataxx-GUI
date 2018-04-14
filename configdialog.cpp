#include "configdialog.h"
#include "ui_configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);

    newPlayer1Name = "Player1";
    newPlayer2Name = "Player2";
    player1IsAI = false;
    ui->comboBoxPlayer1AIDifficulty->setDisabled(true);
    player2IsAI = false;
    ui->comboBoxPlayer2AIDifficulty->setDisabled(true);
    player1AIDifficulty = Ataxx::Easy;
    player2AIDifficulty = Ataxx::Easy;

    connect(ui->player1Edit, SIGNAL(textChanged(QString)),
            this, SLOT(refreshNewPlayer1Name(QString)));
    connect(ui->player2Edit, SIGNAL(textChanged(QString)),
            this, SLOT(refreshNewPlayer2Name(QString)));
    connect(ui->checkBoxPlayer1UseAI, SIGNAL(toggled(bool)),
            this, SLOT(refreshPlayer1IsAI(bool)));
    connect(ui->checkBoxPlayer2UseAI, SIGNAL(toggled(bool)),
            this, SLOT(refreshPlayer2IsAI(bool)));
    connect(ui->comboBoxPlayer1AIDifficulty, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(refreshPlayer1AIDifficulty(QString)));
    connect(ui->comboBoxPlayer2AIDifficulty, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(refreshPlayer2AIDifficulty(QString)));

}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::refreshNewPlayer1Name(QString str)
{
    newPlayer1Name = str;
}

void ConfigDialog::refreshNewPlayer2Name(QString str)
{
    newPlayer2Name = str;
}

void ConfigDialog::refreshPlayer1IsAI(bool checked)
{
    if (checked) {
        player1IsAI = true;
        ui->player1Edit->setDisabled(true);
        ui->comboBoxPlayer1AIDifficulty->setDisabled(false);
        newPlayer1Name = ui->comboBoxPlayer1AIDifficulty->currentText();
    } else {
        player1IsAI = false;
        ui->player1Edit->setDisabled(false);
        ui->comboBoxPlayer1AIDifficulty->setDisabled(true);
        newPlayer1Name = ui->player1Edit->text();
    }
}

void ConfigDialog::refreshPlayer2IsAI(bool checked)
{
    if (checked) {
        player2IsAI = true;
        ui->player2Edit->setDisabled(true);
        ui->comboBoxPlayer2AIDifficulty->setDisabled(false);
        newPlayer2Name = ui->comboBoxPlayer2AIDifficulty->currentText();
    } else {
        player2IsAI = false;
        ui->player2Edit->setDisabled(false);
        ui->comboBoxPlayer2AIDifficulty->setDisabled(true);
        newPlayer2Name = ui->player2Edit->text();
    }
}

void ConfigDialog::refreshPlayer1AIDifficulty(QString diff)
{
    if (diff == "Easy AI") {
        player1AIDifficulty = Ataxx::Easy;
    } else if (diff == "Medium AI") {
        player1AIDifficulty = Ataxx::Medium;
    } else if (diff == "Hard AI") {
        player1AIDifficulty = Ataxx::Hard;
    } else if (diff == "Insane AI") {
        player1AIDifficulty = Ataxx::Insane;
    } else {
        player1AIDifficulty = Ataxx::Medium;
    }
    newPlayer1Name = diff;
}

void ConfigDialog::refreshPlayer2AIDifficulty(QString diff)
{
    if (diff == "Easy AI") {
        player2AIDifficulty = Ataxx::Easy;
    } else if (diff == "Medium AI") {
        player2AIDifficulty = Ataxx::Medium;
    } else if (diff == "Hard AI") {
        player2AIDifficulty = Ataxx::Hard;
    } else if (diff == "Insane AI") {
        player2AIDifficulty = Ataxx::Insane;
    } else {
        player2AIDifficulty = Ataxx::Medium;
    }
    newPlayer2Name = diff;
}
