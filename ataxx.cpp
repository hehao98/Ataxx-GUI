#include <QGridLayout>
#include <QSignalMapper>
#include <QFont>
#include <QMessageBox>
#include <QTime>
#include <QtGlobal>
#include <fstream>
#include "ataxx.h"
#include "mainwindow.h"

Ataxx::Ataxx(QWidget *parent) : QWidget(parent)
{
    QGridLayout *gridLayout = new QGridLayout();
    QSignalMapper *mapper = new QSignalMapper(this);

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            QPushButton *button = new QPushButton();
            QFont font;
            font.setPixelSize(button->size().width() / 11);
            button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            button->setFont(font);
            button->setText("  ");
            button->setStyleSheet("QPushButton{"
                                  "border:2px solid grey;"
                                  "border-radius:0px}");

            gridLayout->addWidget(button, i, j);
            gridInterface.append(button);
            mapper->setMapping(button, gridInterface.count() - 1);
            connect(button, SIGNAL(clicked(bool)), mapper, SLOT(map()));
        }
    }
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(handleButtonClick(int)));
    connect(this, SIGNAL(gameOver(Player)), this, SLOT(handleGameOver(Player)));
    connect(this, SIGNAL(statusChanged()), this, SLOT(checkForAIAction()));
    setLayout(gridLayout);

    initGrid(Player1);
    currMoveStatus = ChoosingStart;
}

Ataxx::~Ataxx()
{

}

void Ataxx::initGrid(Ataxx::Player firstPlayer)
{
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            grid[i][j] = None;
        }
    }
    grid[0][0] = grid[ROW - 1][COLUMN - 1] = Player1;
    grid[0][COLUMN - 1] = grid[ROW - 1][0] = Player2;
    setGridInterface();

    currPlayer = firstPlayer;
    currMoveStatus = ChoosingStart;
}

void Ataxx::setGridInterface()
{
    for (int i = 0; i < ROW*COLUMN; i++) {
        gridInterface.at(i)->setStyleSheet("QPushButton{"
                                           "border:2px solid grey;"
                                           "border-radius:0px}");
        switch(grid[i / ROW][i % COLUMN]) {
        case Player1:
            gridInterface.at(i)->setText("●");
            break;
        case Player2:
            gridInterface.at(i)->setText("○");
            break;
        default:
            gridInterface.at(i)->setText("  ");
        }
    }
    update();
}

void Ataxx::showHint(Ataxx::Move m)
{
    int startID = m.startX * ROW + m.startY;
    int endID = m.endX * ROW + m.endY;
    gridInterface.at(startID)->setStyleSheet("QPushButton{"
                                       "border:2px solid red;"
                                       "border-radius:0px}");
    gridInterface.at(endID)->setStyleSheet("QPushButton{"
                                       "border:2px solid red;"
                                       "border-radius:0px}");
}

bool Ataxx::hasMove(int startX, int startY) const
{
    if (grid[startX][startY] != Player1 && grid[startX][startY] != Player2)
        return false;

    for (int i = startX - 2; i <= startX + 2; i++) {
        for (int j = startY - 2; j <= startY + 2; j++) {
            if (isInGrid(i, j) && grid[i][j] == None) {
                return true;
            }
        }
    }

    return false;
}

bool Ataxx::isValidMove(int startX, int startY, int endX, int endY) const
{
    if (isInGrid(endX, endY) && grid[endX][endY] == None
            && abs(endX - startX) <= 2 && abs(endY - startY) <= 2)
        return true;
    return false;
}

void Ataxx::executeMove(Ataxx::Move move)
{
    Player opponent = (currPlayer == Player1) ? Player2 : Player1;
    grid[move.endX][move.endY] = grid[move.startX][move.startY];
    for (int i = move.endX - 1; i <= move.endX + 1; i++) {
        for (int j = move.endY - 1; j <= move.endY + 1; j++) {
            if (isInGrid(i, j) && grid[i][j] == opponent) {
                grid[i][j] = currPlayer;
            }
        }
    }
    if (abs(move.endX - move.startX) == 2 || abs(move.endY - move.startY) == 2)
        grid[move.startX][move.startY] = None;
}

Ataxx::Player Ataxx::checkWinCondition() const
{
    Player opponent = (currPlayer == Player1) ? Player2 : Player1;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (grid[i][j] == opponent && hasMove(i, j)) {
                return Invalid;
            }
        }
    }
    if (getScore(currPlayer) >= getScore(opponent))
        return currPlayer;
    else
        return opponent;
}

Ataxx::Move Ataxx::findNextMove(Ataxx::Player player, Ataxx::AI_Difficulty difficulty)
{
    switch (difficulty) {
    case Easy:
        return easyAI(player);
    case Medium:
        return mediumAI(player);
    case Hard:
        return hardAI(player);
    case Insane:
        return insaneAI(player);
    }
    return mediumAI(player);
}

Ataxx::Move Ataxx::easyAI(Ataxx::Player player)
{
    Move candidates[100];
    int top = 0;

    Grid g;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            g.grid[i][j] = grid[i][j];
        }
    }


    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (grid[i][j] == player && hasMove(i, j)) {
                for (int k = i - 2; k <= i + 2; k++) {
                    for (int l = j - 2; l <= j + 2; l++) {
                        if (isInGrid(k, l) && isValidMove(i, j, k, l)) {
                            Move m = {i, j, k, l};
                            Grid child = g;
                            executeMove(m, child, player);
                            int prev = getScore(player) - getScore(getOpponent(player));
                            int gain = getScore(player, child) - getScore(getOpponent(player), child);
                            if (top < 100 && gain - prev > 0)
                                candidates[top++] = m;
                        }
                    }
                }
            }
        }
    }

    //Initialize random generator
    static QTime last = QTime::currentTime();
    QTime curr = QTime::currentTime();
    int seed = curr.msecsTo(last);
    qsrand(seed);

    //uses the random number
    int id = qrand() % top;

    //refresh last;
    last = curr;

    return candidates[id];
}

Ataxx::Move Ataxx::mediumAI(Ataxx::Player player)
{
    Move candidates[100];
    int top = 0, maxGain = -10000;

    Grid g;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            g.grid[i][j] = grid[i][j];
        }
    }

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (g.grid[i][j] == player && hasMove(i, j, g)) {
                for (int k = i - 2; k <= i + 2; k++) {
                    for (int l = j - 2; l <= j + 2; l++) {
                        if (isInGrid(k, l) && isValidMove(i, j, k, l, g)) {
                            Move m = {i, j, k, l};
                            Grid child = g;
                            executeMove(m, child, player);
                            int gain = getScore(player, child) - getScore(getOpponent(player), child);
                            if (gain > maxGain) {
                                maxGain = gain;
                                candidates[0] = m;
                                top = 1;
                            } else if (gain == maxGain) {
                                candidates[top] = m;
                                top++;
                            }
                        }
                    }
                }
            }
        }
    }
    //Initialize random generator
    static QTime last = QTime::currentTime();
    QTime curr = QTime::currentTime();
    int seed = curr.msecsTo(last);
    qsrand(seed);

    //uses the random number
    int id = qrand() % top;

    //refresh last;
    last = curr;

    return candidates[id];
}

Ataxx::Move Ataxx::hardAI(Ataxx::Player player)
{
    Move candidates[100];
    int top = 0, maxGain = -10000;

    Grid g;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            g.grid[i][j] = grid[i][j];
        }
    }

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (g.grid[i][j] == player && hasMove(i, j, g)) {
                for (int k = i - 2; k <= i + 2; k++) {
                    for (int l = j - 2; l <= j + 2; l++) {
                        if (isInGrid(k, l) && isValidMove(i, j, k, l, g)) {
                            Move m = {i, j, k, l};
                            Grid child = g;
                            executeMove(m, child, player);
                            int gain = calculateGain(child, 1,
                                       -10000, +10000, getOpponent(player), player);
                            if (gain > maxGain) {
                                maxGain = gain;
                                candidates[0] = m;
                                top = 1;
                            } else if (gain == maxGain) {
                                candidates[top] = m;
                                top++;
                            }
                        }
                    }
                }
            }
        }
    }
    //Initialize random generator
    static QTime last = QTime::currentTime();
    QTime curr = QTime::currentTime();
    int seed = curr.msecsTo(last);
    qsrand(seed);

    //uses the random number
    int id = qrand() % top;

    //refresh last;
    last = curr;

    return candidates[id];
}

Ataxx::Move Ataxx::insaneAI(Ataxx::Player player)
{
    Move candidates[100];
    int top = 0, maxGain = -10000;

    Grid g;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            g.grid[i][j] = grid[i][j];
        }
    }

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (g.grid[i][j] == player && hasMove(i, j, g)) {
                for (int k = i - 2; k <= i + 2; k++) {
                    for (int l = j - 2; l <= j + 2; l++) {
                        if (isInGrid(k, l) && isValidMove(i, j, k, l, g)) {
                            Move m = {i, j, k, l};
                            Grid child = g;
                            executeMove(m, child, player);
                            int gain = calculateGain(child, 3,
                                       -10000, +10000, getOpponent(player), player);
                            if (gain > maxGain) {
                                maxGain = gain;
                                candidates[0] = m;
                                top = 1;
                            } else if (gain == maxGain) {
                                candidates[top] = m;
                                top++;
                            }
                        }
                    }
                }
            }
        }
    }
    //Initialize random generator
    static QTime last = QTime::currentTime();
    QTime curr = QTime::currentTime();
    int seed = curr.msecsTo(last);
    qsrand(seed);

    //uses the random number
    int id = qrand() % top;

    //refresh last;
    last = curr;

    return candidates[id];
}

int Ataxx::calculateGain(Ataxx::Grid g, int depth, int alpha, int beta,
                         Ataxx::Player currPlayer, Ataxx::Player maximizingPlayer)
{
    if (depth == 0) {
        return getScore(maximizingPlayer, g) - getScore(getOpponent(maximizingPlayer), g);
    }

    if (currPlayer == maximizingPlayer) {
        int v = -10000;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COLUMN; j++) {
                if (g.grid[i][j] == currPlayer && hasMove(i, j, g)) {
                    for (int k = i - 2; k <= i + 2; k++) {
                        for (int l = j - 2; l <= j + 2; l++) {
                            if (isInGrid(k, l) && isValidMove(i, j, k, l, g)) {
                                Grid child = g;

                                Move move = {i, j, k, l};
                                executeMove(move, child, currPlayer);

                                int gain = calculateGain(child, depth - 1,
                                           alpha, beta, getOpponent(currPlayer), maximizingPlayer);
                                if (gain > v)
                                    v = gain;
                                if (v > alpha)
                                    alpha = v;
                                if (beta <= alpha)
                                    goto END_OF_LOOP_1;
                            }
                        }
                    }
                }
            }
        }
        END_OF_LOOP_1:
        return v;
    } else {
        int v = 10000;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COLUMN; j++) {
                if (g.grid[i][j] == currPlayer && hasMove(i, j, g)) {
                    for (int k = i - 2; k <= i + 2; k++) {
                        for (int l = j - 2; l <= j + 2; l++) {
                            if (isInGrid(k, l) && isValidMove(i, j, k, l, g)) {
                                Grid child = g;
                                Move move = {i, j, k, l};
                                executeMove(move, child, currPlayer);
                                int gain = calculateGain(child, depth - 1,
                                           alpha, beta, getOpponent(currPlayer), maximizingPlayer);
                                if (gain < v)
                                    v = gain;
                                if (v < beta)
                                    beta = v;
                                if (beta <= alpha)
                                    goto END_OF_LOOP_2;
                            }
                        }
                    }
                }
            }
        }
        END_OF_LOOP_2:
        return v;
    }
}

bool Ataxx::hasMove(int startX, int startY, const Ataxx::Grid &g) const
{
    if (g.grid[startX][startY] != Player1 && g.grid[startX][startY] != Player2)
        return false;

    for (int i = startX - 2; i <= startX + 2; i++) {
        for (int j = startY - 2; j <= startY + 2; j++) {
            if (isInGrid(i, j) && g.grid[i][j] == None) {
                return true;
            }
        }
    }

    return false;
}

bool Ataxx::isValidMove(int startX, int startY, int endX, int endY, const Ataxx::Grid &g) const
{
    if (isInGrid(endX, endY) && g.grid[endX][endY] == None
            && abs(endX - startX) <= 2 && abs(endY - startY) <= 2)
        return true;
    return false;
}

void Ataxx::executeMove(Ataxx::Move move, Ataxx::Grid &g, Ataxx::Player player)
{
    Player opponent = (player == Player1) ? Player2 : Player1;
    g.grid[move.endX][move.endY] = g.grid[move.startX][move.startY];
    for (int i = move.endX - 1; i <= move.endX + 1; i++) {
        for (int j = move.endY - 1; j <= move.endY + 1; j++) {
            if (isInGrid(i, j) && g.grid[i][j] == opponent) {
                g.grid[i][j] = player;
            }
        }
    }
    if (abs(move.endX - move.startX) == 2 || abs(move.endY - move.startY) == 2)
        g.grid[move.startX][move.startY] = None;
}

int Ataxx::getScore(Ataxx::Player player, Ataxx::Grid g) const
{
    int score = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (g.grid[i][j] == player) {
                score++;
            }
        }
    }
    return score;
}

int Ataxx::getScore(Ataxx::Player player) const
{
    int score = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            if (grid[i][j] == player) {
                score++;
            }
        }
    }
    return score;
}

void Ataxx::save()
{
    using std::endl;
    std::ofstream save("ataxx.save");
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            save << (int)grid[i][j] << " ";
        }
        save << endl;
    }
    save << (int)currMoveStatus << endl;
    save << currMove.startX << " " << currMove.startY << " "
         << currMove.endX << " " << currMove.endY << endl;
    save << (int)currPlayer << endl;
    save << (int)player1IsAI << " " << (int)player1Difficulty << endl;
    save << (int)player2IsAI << " " << (int)player2Difficulty << endl;
    QMessageBox::information(this, "Save", "Save Success!");
}

void Ataxx::load()
{
    std::ifstream load("ataxx.save");
    int newGrid[ROW][COLUMN], newMoveStatus, newPlayer,
            new1A, new1AD, new2A, new2AD;

    if (load.is_open()) {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < ROW; j++) {
                load >> newGrid[i][j];
            }
        }
        load >> newMoveStatus;
        load >> currMove.startX >> currMove.startY >> currMove.endX >> currMove.endY;
        load >> newPlayer;
        load >> new1A >> new1AD;
        load >> new2A >> new2AD;

        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < ROW; j++) {
                grid[i][j] = (Player)newGrid[i][j];
            }
        }
        currMoveStatus = (MoveStatus) newMoveStatus;
        currPlayer = (Player) newPlayer;
        player1IsAI = new1A;
        player1Difficulty = (AI_Difficulty) new1AD;
        player2IsAI = new2A;
        player2Difficulty = (AI_Difficulty) new2AD;

        QMessageBox::information(this, "Load", "Load Success!");
    } else {
        QMessageBox::information(this, "Load", "Load Failure!");
    }
}

void Ataxx::handleButtonClick(int id)
{
    int currX = id / ROW, currY = id % COLUMN;

    if (currMoveStatus == ChoosingStart) {
        if (grid[currX][currY] != currPlayer)
            return;
        if (!hasMove(currX, currY))
            return;

        //some visual effect here
        gridInterface.at(id)->setStyleSheet("QPushButton{"
                                            "border:2px solid black;"
                                            "border-radius:0px}");
        for (int i = currX - 2; i <= currX + 2; i++) {
            for (int j = currY - 2; j <= currY + 2; j++) {
                if (isInGrid(i, j) && isValidMove(currX, currY, i, j)) {
                    if (abs(currX - i) <= 1 && abs(currY - j) <= 1) {
                        gridInterface.at(i * ROW + j)->setStyleSheet("QPushButton{"
                                                                     "border:2px solid blue;"
                                                                     "border-radius:0px}");
                    } else {
                        gridInterface.at(i * ROW + j)->setStyleSheet("QPushButton{"
                                                                     "border:2px solid green;"
                                                                     "border-radius:0px}");
                    }
                }
            }
        }

        currMove.startX = currX;
        currMove.startY = currY;
        currMoveStatus = ChoosingDestination;

    } else if (currMoveStatus == ChoosingDestination) {
        if (isValidMove(currMove.startX, currMove.startY, currX, currY)) {
            currMove.endX = currX;
            currMove.endY = currY;
            executeMove(currMove);
            setGridInterface();

            Player temp = checkWinCondition();
            if (temp != Invalid) {
                emit gameOver(temp);
                return;
            }

            changePlayer();
            currMoveStatus = ChoosingStart;

            emit statusChanged();
        } else {
            return;
        }
    }

    if (player1IsAI && player2IsAI)
        emit statusChanged();
}

void Ataxx::handleGameOver(Ataxx::Player player)
{
    QString message;
    if (player == Ataxx::Player1) {
        message = "The black side wins!";
    } else {
        message = "The white side wins!";
    }
    QMessageBox::information(this, "Info", message);
}

void Ataxx::checkForAIAction()
{
    if ((currPlayer == Player1 && player1IsAI) || (currPlayer == Player2 && player2IsAI)) {
        AI_Difficulty currDiff = (currPlayer == Player1) ? player1Difficulty : player2Difficulty;

        Move move = findNextMove(currPlayer, currDiff);
        executeMove(move);
        setGridInterface();

        Player temp = checkWinCondition();
        if (temp != Invalid) {
            emit gameOver(temp);
            return;
        }

        changePlayer();
        currMoveStatus = ChoosingStart;

        emit statusChanged();
    }
}
