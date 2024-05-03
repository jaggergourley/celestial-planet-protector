#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include "game.h"

class Ship;

struct PlayerAmmo
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

struct AlienAmmo
{
    int y;
    int x;
    int dy;
    int dx;
};

struct Alien
{
    int y;
    int x;
    int fireCooldown; // Time until next shot can be fired
};

struct Ammo
{
    int y;
    int x;
};

struct Shield
{
    int y;
    int x;
};

class GameState
{
public:
    GameState();
    void reset();

    void generateShieldBoost();
    void generateAmmoBoost();
    void drawBoosts();
    void checkShipBoostCollision(Ship &ship);

    void generatePlayerShot(Ship &ship);
    void updatePlayerShots();

    void generateAsteroid();
    void updateAsteroids();

    void generateAlien();
    void updateAliens(Ship &ship);
    void drawAliens();
    void generateAlienShot(Ship &ship);
    void updateAlienShots();

    void checkShipAsteroidCollision(Ship &ship);
    void checkPlayerAmmoAsteroidCollision(Ship &ship);
    bool checkShipAlienCollision(Ship &ship);
    void checkShipAlienAmmoCollision(Ship &ship);
    void checkAlienPlayerAmmoCollision(Ship &ship);
    void checkAlienAsteroidCollision();

private:
    std::vector<PlayerAmmo> playerShots;
    std::vector<AlienAmmo> alienShots;
    std::vector<Asteroid> asteroids;
    std::vector<Alien> aliens;
    std::vector<Ammo> ammoBoosts;
    std::vector<Shield> shieldBoosts;
};

#endif