#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>
#include <ncurses.h>

#include "game.h"
#include "game_state.h"

#define HEIGHT 24
#define WIDTH 80

Ship initShip()
{
    Ship ship;
    ship.y = HEIGHT / 2;
    ship.x = WIDTH / 2;
    ship.direction = 0;
    ship.shield = 50; // Start with 50% shield
    ship.health = 100;
    ship.ammo = 50; // Start with 50% full ammo
    ship.score = 0;

    return ship;
}

void resetShip(Ship &ship)
{
    ship.y = HEIGHT / 2;
    ship.x = WIDTH / 2;
    ship.direction = 0;
    ship.shield = 50; // Start with 50% shield
    ship.health = 100;
    ship.ammo = 50; // Start with 50% full ammo
    ship.score = 0;
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

void infoScreen()
{
    mvprintw(6, 15, "Get points and survive for as long as you can!\n");
    mvprintw(7, 15, "Move using arrow keys or WASD\n");
    mvprintw(8, 15, "Fire shots using SPACE\n");
    mvprintw(9, 15, "Gather shield (%%) & ammo (+) boosts\n");
    mvprintw(10, 15, "Every second you survive is worth +1 point\n");
    mvprintw(11, 15, "Destroying an asteroid (@) is worth +5 points\n");
    mvprintw(12, 15, "Destroying an alien ship (0) is worth +10 points\n");
    mvprintw(13, 15, "Don't allow alien ships to reach you\n");
    mvprintw(14, 15, "Press 'q' to quit\n");
    mvprintw(15, 15, "Press space to begin!");

    // Wait for user to press space
    int ch;
    while ((ch = getch()) != ' ')
        ;
}

bool gameOver(Ship &ship)
{
    clear();
    mvprintw(8, 30, "Game Over!");
    mvprintw(9, 30, "Score: %d", ship.score);
    mvprintw(10, 30, "Press 'r' to restart");
    mvprintw(11, 30, "Press 'q' to quit");
    refresh();

    char ch;
    while (true)
    {
        ch = getch();
        if (ch == 'r')
        {
            return true;
        }
        else if (ch == 'q')
        {
            return false;
        }
    }
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

    mvprintw(0, 1, "HP: ");
    mvprintw(0, 21, "Shield: ");
    mvprintw(0, 45, "Ammo: ");

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

    gameState.generateShieldBoost();
    gameState.generateAmmoBoost();
    gameState.drawBoosts();
    gameState.checkShipBoostCollision(ship);

    gameState.updatePlayerShots();
    gameState.updateAsteroids();

    gameState.drawAliens();
    gameState.generateAlienShot(ship);
    gameState.updateAlienShots();

    gameState.checkShipAsteroidCollision(ship);
    gameState.checkPlayerAmmoAsteroidCollision(ship);
    gameState.checkAlienPlayerAmmoCollision(ship);
    gameState.checkShipAlienAmmoCollision(ship);
    gameState.checkAlienAsteroidCollision();

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
    const std::chrono::milliseconds gameUpdateInterval(100);   // 50 milliseconds
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

            count += 1;
            // Generate asteroids more frequently as time goes on

            int asteroidFrequency = std::max(50 - count / 5, 5);
            if (rand() % asteroidFrequency == 0)
            {
                gameState.generateAsteroid();
            }

            // // Generate aliens more frequently as time goes on
            int alienFrequency = std::max(50 - count / 10, 5);
            if (rand() % alienFrequency == 0)
            {
                gameState.generateAlien();
            }

            updateGameState(ship, gameState);
            // Update aliens once every 4 steps
            if (count % 4 == 0)
            {
                gameState.updateAliens(ship);
            }

            if (ship.health <= 0 || !gameState.checkShipAlienCollision(ship))
            {
                bool restart = gameOver(ship);
                if (!restart)
                {
                    return; // Exit game loop
                }
                else
                {
                    // Reset everything to initial conditions
                    gameState.reset();
                    resetShip(ship);
                    count = 0;
                }
            }

            lastGameUpdateTime = currentTime;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Delay
    }

    nodelay(stdscr, FALSE);
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    GameState gameState;
    Ship ship = initShip();

    initNcurses();

    // Define color pairs and set default color
    init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(COLOR_PAIR_DEFAULT));

    infoScreen();

    drawStatusBars(ship);
    drawShip(ship);

    gameLoop(ship, gameState);

    // End ncurses
    endwin();
    return 0;
}