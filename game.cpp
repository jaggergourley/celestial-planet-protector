#include <iostream>
#include <algorithm>
#include <ncurses.h>

#include "game.h"

#define HEIGHT 24
#define WIDTH 80

void gameLoop()
{
    // Initial ship position
    int ship_y = HEIGHT / 2;
    int ship_x = WIDTH / 2;

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
            ship_y = std::max(ship_y - 1, 0);
            break;
        case 's':
            ship_y = std::min(ship_y + 1, HEIGHT - 1);
            break;
        case 'a':
            ship_x = std::max(ship_x - 1, 0);
            break;
        case 'd':
            ship_x = std::min(ship_x + 1, WIDTH - 1);
            break;
        }

        // Draw ship
        mvprintw(ship_y, ship_x, "A");

        refresh();
    }
}

int main()
{
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Hide cursor

    gameLoop();

    // End ncurses
    endwin();
    return 0;
}