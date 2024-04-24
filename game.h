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

Ship initShip();
void createShot(const Ship &ship);
void updateShot(Ammo *shot);
void updateAllShots();
bool isShotOutOfBounds(const Ammo *shot);
void renderShot(const Ammo *shot);
void drawShip(const Ship &ship);
void drawStatusBars(const Ship &ship);
void shoot(Ship &ship);

void gameLoop(Ship &ship);
void initNcurses();

#endif