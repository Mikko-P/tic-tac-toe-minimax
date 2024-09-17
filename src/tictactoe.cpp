#include "tictactoe.h"
#include <iostream>
#include <algorithm>
#include <cmath>

TicTacToe::TicTacToe() : window(nullptr), renderer(nullptr), board(BOARD_SIZE, std::vector<Player>(BOARD_SIZE, Player::NONE)), currentPlayer(Player::X) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

TicTacToe::~TicTacToe() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool TicTacToe::checkWin(Player player) {
    // rows, colums
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
            (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
            return true;
        }
    }
    // x pattern
    if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
        (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
        return true;
    }
    return false;
}

bool TicTacToe::isBoardFull() {
    for (const auto& row : board) {
        for (const auto& cell : row) {
            if (cell == Player::NONE) {
                return false;
            }
        }
    }
    return true;
}

// Learn more about the algorithm
// https://en.wikipedia.org/wiki/Minimax
// https://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-1-introduction/
int TicTacToe::minimax(int depth, bool isMaximizing) {
    // Player won, don't do this
    if (checkWin(Player::X))
        return -10 + depth;

    // Machine won, do this
    if (checkWin(Player::O))
        return 10 - depth;

    // draw, don't do this
    if (isBoardFull())
        return 0;
    
    // Find the best move by recursively calling the function
    // and checking moves for both the player and the machine
    int bestScore = isMaximizing ? -1000 : 1000;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == Player::NONE) {
                board[i][j] = isMaximizing ? Player::O : Player::X;
                int score = minimax(depth + 1, !isMaximizing);
                //std::cout << score << "\n";
                board[i][j] = Player::NONE;
                bestScore = isMaximizing ? std::max(score, bestScore) : std::min(score, bestScore);
            }
        }
    }
    return bestScore;
}

// Play a move -> Get score for it -> remove the move
// -> play another move until we've played all moves possible
// keeping the best move in mind
Move TicTacToe::getBestMove() {
    int bestScore = -1000;
    Move bestMove = {-1, -1};
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == Player::NONE) {
                board[i][j] = Player::O;
                int score = minimax(0, false);
                board[i][j] = Player::NONE;
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = {i, j};
                }
            }
        }
    }
    return bestMove;
}

// There's no built in function for drawing circles in SDL
// so I took one from the internet
// https://stackoverflow.com/questions/38334081/how-to-draw-circles-arcs-and-vector-graphics-in-sdl
void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void TicTacToe::drawBoard() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // draw the board
    for (int i = 1; i < BOARD_SIZE; ++i) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, WINDOW_WIDTH, i * CELL_SIZE);
    }

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            SDL_Rect cell = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            if (board[i][j] == Player::X) {
                // draw x
                SDL_RenderDrawLine(renderer, cell.x, cell.y, cell.x + cell.w, cell.y + cell.h);
                SDL_RenderDrawLine(renderer, cell.x + cell.w, cell.y, cell.x, cell.y + cell.h);
            } else if (board[i][j] == Player::O) {
                // drawing circel
                int centerX = cell.x + cell.w / 2;
                int centerY = cell.y + cell.h / 2;
                int radius = cell.w / 2 - 10;
                drawCircle(renderer, centerX, centerY, radius);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void TicTacToe::run() {
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN && currentPlayer == Player::X) {
                int x;
                int y;
                SDL_GetMouseState(&x, &y);
                int row = y / CELL_SIZE;
                int col = x / CELL_SIZE;

                if (board[row][col] == Player::NONE) {
                    board[row][col] = Player::X;
                    if (checkWin(Player::X)) {
                        std::cout << "x won" << "\n";
                        quit = true;
                    } else if (isBoardFull()) {
                        std::cout << "draw" << "\n";
                        quit = true;
                    } else {
                        currentPlayer = Player::O;
                    }
                }
            }
        }

        if (currentPlayer == Player::O && !quit) {
            Move bestMove = getBestMove();
            board[bestMove.row][bestMove.col] = Player::O;
            if (checkWin(Player::O)) {
                std::cout << "o won" << "\n";
                quit = true;
            } else if (isBoardFull()) {
                std::cout << "draw" << "\n";
                quit = true;
            } else {
                currentPlayer = Player::X;
            }
        }

        drawBoard();
        SDL_Delay(100);
    }

    SDL_Delay(2000);
}
