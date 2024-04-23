#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <ncurses.h>

#include "game.h"

#define HEIGHT 24
#define WIDTH 80

enum ColorPairs
{
    COLOR_PAIR_DEFAULT,
    COLOR_PAIR_RED,
    COLOR_PAIR_GREEN,
    COLOR_PAIR_YELLOW
};

// Define structure for the ship
struct Ship
{
    int y;                // Y-coord
    int x;                // X-coord
    int direction;        // Direction ship is facing (0: up, 1: right, 2: down, 3: left)
    char shapes[4][3][3]; // Represent shape of ship
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

Ship initShip(int y, int x)
{
    Ship ship;
    ship.y = y;
    ship.x = x;
    ship.direction = 0; // Start facing up
    ship.shield = 50;   // Start with 50% shield
    ship.health = 100;
    ship.ammo = 100;
    ship.score = 0;

    char upShape[3][3] = {
        {' ', 'X', ' '},
        {' ', 'X', ' '},
        {'X', 'X', 'X'}};

    char rightShape[3][3] = {
        {'X', ' ', ' '},
        {'X', 'X', 'X'},
        {'X', ' ', ' '}};

    char downShape[3][3] = {
        {'X', 'X', 'X'},
        {' ', 'X', ' '},
        {' ', 'X', ' '}};

    char leftShape[3][3] = {
        {' ', ' ', 'X'},
        {'X', 'X', 'X'},
        {' ', ' ', 'X'}};

    std::copy(&upShape[0][0], &upShape[0][0] + 9, &ship.shapes[0][0][0]);
    std::copy(&rightShape[0][0], &rightShape[0][0] + 9, &ship.shapes[1][0][0]);
    std::copy(&downShape[0][0], &downShape[0][0] + 9, &ship.shapes[2][0][0]);
    std::copy(&leftShape[0][0], &leftShape[0][0] + 9, &ship.shapes[3][0][0]);

    return ship;
}

Ammo *createShot(const Ship &ship)
{
    // Allocate memory for shot
    Ammo *shot = new Ammo;

    // Calc initial position and direction based on ship's info

    return shot;
}

void destroyShot(Ammo *shot)
{
    // Free memory for shot
    delete shot;
}

void drawShip(const Ship &ship)
{
    // Draw ship at current postion based on direction
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            mvprintw(ship.y + i, ship.x + j, "%c", ship.shapes[ship.direction][i + 1][j + 1]);
        }
    }
}

void drawStatusBars(const Ship &ship)
{
    // Define width of each bar
    int barWidth = 12;

    attron(COLOR_PAIR(COLOR_PAIR_GREEN));
    mvprintw(0, 1, "HP: [%-*s]", barWidth, std::string((ship.health * barWidth) / 100, '=').c_str());
    attroff(COLOR_PAIR(COLOR_PAIR_GREEN));

    attron(COLOR_PAIR(COLOR_PAIR_YELLOW));
    mvprintw(0, 21, "Shield: [%-*s]", barWidth, std::string((ship.shield * barWidth) / 100, '=').c_str());
    attroff(COLOR_PAIR(COLOR_PAIR_YELLOW));

    attron(COLOR_PAIR(COLOR_PAIR_RED));
    mvprintw(0, 45, "Ammo: [%-*s]", barWidth, std::string((ship.ammo * barWidth) / 100, '=').c_str());
    attroff(COLOR_PAIR(COLOR_PAIR_RED));

    mvprintw(0, 67, "Score: %d", ship.score);
}

void shoot(Ship &ship)
{
    // Check if ship has enough ammo
    if (ship.ammo > 0)
    {
        ship.ammo--;
    }
}

void gameLoop(Ship &ship)
{
    // Main loop
    int ch;
    // Press 'q' to quit
    while ((ch = getch()) != 'q')
    {
        // Clear the screen
        clear();

        // Check for input
        switch (ch)
        {
        case 'w':
            ship.y = std::max(ship.y - 1, 2);
            break;
        case 's':
            ship.y = std::min(ship.y + 1, HEIGHT - 2);
            break;
        case 'a':
            ship.x = std::max(ship.x - 1, 1);
            break;
        case 'd':
            ship.x = std::min(ship.x + 1, WIDTH - 2);
            break;
        case KEY_UP:
            ship.direction = 0; // Up
            break;
        case KEY_RIGHT:
            ship.direction = 1; // Right
            break;
        case KEY_DOWN:
            ship.direction = 2; // Down
            break;
        case KEY_LEFT:
            ship.direction = 3; // Left
            break;
        case ' ' || 32:
            shoot(ship);
            break;
        }

        drawShip(ship);
        drawStatusBars(ship);

        refresh();
    }
}

void initNcurses()
{
    // Initialize ncurses
    initscr();
    start_color();
    init_pair(COLOR_PAIR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_PAIR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide cursor
}

int main()
{

    Ship ship = initShip(HEIGHT / 2, WIDTH / 2);

    initNcurses();

    // Define color pairs and set default color
    init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(COLOR_PAIR_DEFAULT));

    drawShip(ship);

    gameLoop(ship);

    // End ncurses
    endwin();
    return 0;
}