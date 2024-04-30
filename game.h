#ifndef GAME_H
#define GAME_H

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

Ship initShip();
void initNcurses();
void drawShip(const Ship &ship);
void drawStatusBars(const Ship &ship);

void generatePlayerShot(std::vector<Ammo> &playerShots, Ship &ship);
void updatePlayerShots(std::vector<Ammo> &playerShots);
void drawPlayerShots(std::vector<Ammo> &playerShots);

void generateAsteroid(std::vector<Asteroid> &asteroids);
void updateAsteroids(std::vector<Asteroid> &asteroids);
void drawAsteroids(const std::vector<Asteroid> &asteroids);

void checkShipAsteroidCollision(Ship &ship, std::vector<Asteroid> &asteroids);

void gameLoop(Ship &ship);

#endif