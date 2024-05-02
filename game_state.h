#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include "fight.h"
#include "game.h"

class Ship;
struct Ammo;
struct Asteroid;

class GameState
{
public:
    GameState();

    void generatePlayerShot(Ship &ship);
    void updatePlayerShots();

    void generateAsteroid();
    void updateAsteroids();

    friend void checkShipAsteroidCollision(Ship &ship, GameState &gameState);
    friend void checkAmmoAsteroidCollision(GameState &gameState);

private:
    std::vector<Ammo> playerShots;
    std::vector<Asteroid> asteroids;
};

#endif