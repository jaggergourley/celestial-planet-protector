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

void createShot(const Ship &ship);
void updateShot(Ammo *shot);
bool isShotOutOfBounds(const Ammo *shot);
void shoot(Ship &ship);
void drawShot(const Ammo *shot);
void updateAllShots();

void gameLoop(Ship &ship);

#endif