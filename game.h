#ifndef GAME_H
#define GAME_H

#include <vector>

#include "fight.h"
#include "game_state.h"

struct Ship
{
    int y;         // Y-coord
    int x;         // X-coord
    int direction; // Direction of shot (0: up, 1: right, 2: down, 3: left)
    int shield;
    int health;
    int ammo;
    int score;
};

Ship initShip();
void initNcurses();
void drawShip(const Ship &ship);
void drawStatusBars(const Ship &ship);

bool handleInput(Ship &ship, GameState &gameState);
void updateGameState(Ship &ship, GameState &gameState);
void gameLoop(Ship &ship);

#endif