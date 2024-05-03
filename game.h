#ifndef GAME_H
#define GAME_H

#include <vector>

#include "game_state.h"

class GameState;

enum ColorPairs
{
    COLOR_PAIR_DEFAULT,
    COLOR_PAIR_RED,
    COLOR_PAIR_GREEN,
    COLOR_PAIR_YELLOW
};

struct Ship
{
    int y;
    int x;
    int direction; // (0: up, 1: right, 2: down, 3: left)
    int shield;
    int health;
    int ammo;
    int score;
};

Ship initShip();
void resetShip(Ship &ship);
void initNcurses();
void infoScreen();
bool gameOver(Ship &ship);

void drawShip(const Ship &ship);
void drawStatusBars(const Ship &ship);

bool handleInput(Ship &ship, GameState &gameState);
void updateGameState(Ship &ship, GameState &gameState);
void gameLoop(Ship &ship, GameState &gameState);

#endif