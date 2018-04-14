#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "ataxx.h"

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();
    QString getNewPlayer1Name() {
        return newPlayer1Name;
    }
    QString getNewPlayer2Name() {
        return newPlayer2Name;
    }
    bool getPlayer1IsAI() {
        return player1IsAI;
    }
    bool getPlayer2IsAI() {
        return player2IsAI;
    }
    Ataxx::AI_Difficulty getPlayer1AIDifficulty() {
        return player1AIDifficulty;
    }
    Ataxx::AI_Difficulty getPlayer2AIDifficulty() {
        return player2AIDifficulty;
    }

private:
    Ui::ConfigDialog *ui;
    QString newPlayer1Name;
    QString newPlayer2Name;
    bool player1IsAI;
    bool player2IsAI;
    Ataxx::AI_Difficulty player1AIDifficulty;
    Ataxx::AI_Difficulty player2AIDifficulty;
public slots:
    void refreshNewPlayer1Name(QString str);
    void refreshNewPlayer2Name(QString str);
    void refreshPlayer1IsAI(bool checked);
    void refreshPlayer2IsAI(bool checked);
    void refreshPlayer1AIDifficulty(QString diff);
    void refreshPlayer2AIDifficulty(QString diff);
};

#endif // CONFIGDIALOG_H
