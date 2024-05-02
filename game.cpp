#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ncurses.h>

#include "game.h"

#define HEIGHT 24
#define WIDTH 80

std::vector<Ammo> playerShots;
std::vector<Asteroid> asteroids;

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

bool handleInput(Ship &ship, GameState &gameState)
{

    int ch = getch();

    // Check for input
    if (ch != ERR)
    {
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
            gameState.generatePlayerShot(ship);
            break;
        case 'q':
            return false; // Quit the game
        }
    }
    return true; // Continue
}

void updateGameState(Ship &ship, GameState &gameState)
{

    clear();

    // Update game state
    drawStatusBars(ship);
    drawShip(ship);

    gameState.updatePlayerShots();
    gameState.updateAsteroids();

    // mvprintw(2, 0, "working after %d iters with %zu asteroids and %zu shots", count, gameState.asteroids.size(), gameState.playerShots.size());

    checkShipAsteroidCollision(ship, gameState);
    checkAmmoAsteroidCollision(gameState);

    refresh();
}

void gameLoop(Ship &ship, GameState &gameState)
{

    int count = 0;
    bool running = true;

    // Set up variables for timing
    auto startTime = std::chrono::steady_clock::now();
    auto lastGameUpdateTime = startTime;
    auto lastScoreUpdateTime = startTime;
    const std::chrono::milliseconds gameUpdateInterval(100);   // 100 milliseconds
    const std::chrono::milliseconds scoreUpdateInterval(1000); // 1 second

    // Enable non-blocking input
    nodelay(stdscr, TRUE);

    while (running)
    {

        running = handleInput(ship, gameState);

        auto currentTime = std::chrono::steady_clock::now();
        // Total time since game started
        auto totalElapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        // Time since last game update
        auto gameTimeChange = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastGameUpdateTime);
        // Time since last score update
        auto scoreTimeChange = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastScoreUpdateTime);

        // Increment score every second
        if (scoreTimeChange >= scoreUpdateInterval)
        {
            ship.score += 1;
            lastScoreUpdateTime = currentTime;
        }

        // Update game state every 0.1 seconds
        if (gameTimeChange >= gameUpdateInterval)
        {
            // count += 1;
            // Generate asteroids more frequently as time goes on
            int asteroidFrequency = std::max(10 - static_cast<int>(totalElapsedTime.count() / 1000), 5);
            if (rand() % asteroidFrequency == 0)
            {
                gameState.generateAsteroid();
            }

            updateGameState(ship, gameState);

            if (ship.health <= 0)
            {
                return;
            }

            lastGameUpdateTime = currentTime;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Delay
    }

    nodelay(stdscr, FALSE);
}

int main()
{
    GameState gameState;
    Ship ship = initShip();

    initNcurses();

    // Define color pairs and set default color
    init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(COLOR_PAIR_DEFAULT));

    drawStatusBars(ship);
    drawShip(ship);

    gameLoop(ship, gameState);

    // End ncurses
    endwin();
    return 0;
}