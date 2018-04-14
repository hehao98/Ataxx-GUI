#ifndef ATAXX_H
#define ATAXX_H

#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QString>

class Ataxx : public QWidget
{
    Q_OBJECT
public:
    enum Player {
        Player1 = 1, Player2 = -1, None = 0, Invalid = 2, Draw = 3
    };
    struct Move {
        int startX, startY, endX, endY;
    };
    enum MoveStatus {
        ChoosingStart, ChoosingDestination
    };
    enum AI_Difficulty {
        Easy = 0, Medium = 1, Hard = 2, Insane = 3
    };

    explicit Ataxx(QWidget *parent = 0);
    ~Ataxx();

    //UI functions
    void setGridInterface();
    void showHint(Move m);

    //game logic functions
    void initGrid(Player firstPlayer);
    void changePlayer() {
        currPlayer = (currPlayer == Player1) ? Player2 : Player1;
    }
    bool hasMove(int startX, int startY) const;
    bool isInGrid(int x, int y) const {
        return (x >= 0 && x < ROW && y >= 0 && y < COLUMN)
                ? true : false;
    }
    bool isValidMove(int startX, int startY, int endX, int endY) const;
    void executeMove(Move move);
    Player checkWinCondition() const;
    int abs(int x) const {
        return x >= 0 ? x : -x;
    }

    //AI-Related functions(including AI-related overloaded functions)
    struct Grid {
        Player grid[7][7];
    };
    Move findNextMove(Player player, AI_Difficulty difficulty);
    Move easyAI(Player player);
    Move mediumAI(Player player);
    Move hardAI(Player player);
    Move insaneAI(Player player);
    int calculateGain(Grid g, int depth, int alpha, int beta,
                      Player currPlayer, Player maximizingPlayer);
    bool hasMove(int startX, int startY, const Grid &g) const;
    bool isValidMove(int startX, int startY, int endX, int endY, const Grid &g) const;
    void executeMove(Move move, Grid &g, Player player);
    int getScore(Player player, Grid g) const;

    //getter functions:
    Player getCurrPlayer() const {
        return currPlayer;
    }
    bool getPlayer1IsAI() const {
        return player1IsAI;
    }
    AI_Difficulty getPlayer1Difficulty() const {
        return player1Difficulty;
    }
    bool getPlayer2IsAI() const {
        return player2IsAI;
    }
    AI_Difficulty getPlayer2Difficulty() const {
        return player2Difficulty;
    }
    Player getOpponent(Player player) {
        return (player == Player1) ? Player2 : Player1;
    }
    int getScore(Player player) const;

    //setter functions
    void setPlayer1IsAI(bool isAI) {
        player1IsAI = isAI;
    }
    void setPlayer2IsAI(bool isAI) {
        player2IsAI = isAI;
    }
    void setPlayer1AIDifficulty(AI_Difficulty newDiff) {
        player1Difficulty = newDiff;
    }
    void setPlayer2AIDifficulty(AI_Difficulty newDiff) {
        player2Difficulty = newDiff;
    }

    //file related functions
    void save();
    void load();
private:
    static const int ROW = 7, COLUMN = 7;
    MoveStatus currMoveStatus;
    Move currMove;
    const int BLACK = (int) Player1, WHITE = (int) Player2;
    Player grid[ROW][COLUMN];
    Player currPlayer;
    QList <QPushButton*> gridInterface;
    bool player1IsAI = false;
    AI_Difficulty player1Difficulty;
    bool player2IsAI = false;
    AI_Difficulty player2Difficulty;

signals:
    void statusChanged();
    void gameOver(Player player);
public slots:
    void handleButtonClick(int id);
    void handleGameOver(Player player);
    void checkForAIAction();
};

#endif // ATAXX_H
