#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <SDL2/SDL.h>
#include <vector>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 800;
constexpr int BOARD_SIZE = 3;
constexpr int CELL_SIZE = WINDOW_WIDTH / BOARD_SIZE;

enum class Player { 
    NONE,
    X,
    O
};

struct Move {
    int row, col;
};

class TicTacToe {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<std::vector<Player>> board;
    Player currentPlayer;

    bool checkWin(Player player);
    bool isBoardFull();
    int minimax(int depth, bool isMaximizing);
    Move getBestMove();
    void drawBoard();

public:
    TicTacToe();
    ~TicTacToe();
    void run();
};

#endif

