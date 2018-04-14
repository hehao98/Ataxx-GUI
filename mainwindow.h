#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ataxx.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;

public slots:
    void startNewGame();
    void loadLastSave();
    void saveGame();
    void generateHintForPlayer1();
    void generateHintForPlayer2();
    void popConfigDialog();
    void popAboutMessage();
    void quitGame();
    void refreshUI();
    void handleGameOver(Ataxx::Player player);
};

#endif // MAINWINDOW_H
