#ifndef FIGHT_H
#define FIGHT_H

#include <vector>
#include "game.h"
#include "game_state.h"

class GameState;

struct Ammo
{
    int y;
    int x;
    int direction; // Direction of shot (0: up, 1: right, 2: down, 3: left)
};

struct Asteroid
{
    int y;
    int x;
    int dy; // Velocity in y direction
    int dx; // Velocity in x direction
};

void checkShipAsteroidCollision(Ship &ship, GameState &gameState);
void checkAmmoAsteroidCollision(GameState &gameState);

#endif