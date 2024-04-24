#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <ncurses.h>

#include "game.h"

#define HEIGHT 24
#define WIDTH 80

std::vector<Ammo *> shots;

enum ColorPairs
{
    COLOR_PAIR_DEFAULT,
    COLOR_PAIR_RED,
    COLOR_PAIR_GREEN,
    COLOR_PAIR_YELLOW
};

Ship initShip()
{
    Ship ship;
    ship.y = HEIGHT / 2;
    ship.x = WIDTH / 2;
    ship.direction = 0;
    ship.shield = 50; // Start with 50% shield
    ship.health = 100;
    ship.ammo = 100;
    ship.score = 0;

    return ship;
}

void createShot(const Ship &ship)
{
    // Allocate memory for shot
    Ammo *newShot = new Ammo{ship.x, ship.y, ship.direction};
    // Add shot to vector
    shots.push_back(newShot);
}

void updateShot(Ammo *shot)
{
    // Move shot based on direction
    switch (shot->direction)
    {
    case 0:
        shot->y--;
        break;
    case 1:
        shot->x++;
        break;
    case 2:
        shot->y++;
        break;
    case 3:
        shot->x--;
        break;
    }
}

void updateAllShots()
{
    for (auto it = shots.begin(); it != shots.end();)
    {
        updateShot(*it);

        if (isShotOutOfBounds(*it))
        {
            delete *it;
            it = shots.erase(it);
        }
        else
        {
            renderShot(*it);
            ++it;
        }
    }
}

bool isShotOutOfBounds(const Ammo *shot)
{
    return (shot->x < 0 || shot->x >= WIDTH - 1 || shot->y < 1 || shot->y >= HEIGHT - 1);
}

void renderShot(const Ammo *shot)
{
    mvprintw(shot->y, shot->x, "*");
}

void drawShip(const Ship &ship)
{
    char symbols[4] = {'^', '>', 'v', '<'};
    // Draw ship at current postion
    mvprintw(ship.y, ship.x, "%c", symbols[ship.direction]);
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
    createShot(ship);
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
        case KEY_UP:
            ship.y = std::max(ship.y - 1, 1);
            ship.direction = 0;
            break;
        case 's':
        case KEY_DOWN:
            ship.y = std::min(ship.y + 1, HEIGHT - 1);
            ship.direction = 2;
            break;
        case 'a':
        case KEY_LEFT:
            ship.x = std::max(ship.x - 1, 0);
            ship.direction = 3;
            break;
        case 'd':
        case KEY_RIGHT:
            ship.x = std::min(ship.x + 1, WIDTH - 1);
            ship.direction = 1;
            break;
        case ' ':
            shoot(ship);
            break;
        }

        drawStatusBars(ship);
        drawShip(ship);
        updateAllShots();

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
    // init_pair(1, COLOR_WHITE, COLOR_WHITE);
    // init_pair(2, COLOR_GREEN, COLOR_GREEN);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide cursor
}

int main()
{

    Ship ship = initShip();

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