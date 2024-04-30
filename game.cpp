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

void generatePlayerShot(std::vector<Ammo> &playerShots, Ship &ship)
{
    // Check if ship has enough ammo
    if (ship.ammo > 0)
    {
        ship.ammo--;
        Ammo shot;
        shot.direction = ship.direction;
        shot.x = ship.x;
        shot.y = ship.y;
        playerShots.push_back(shot);
    }
    else
    {
        return;
    }
}

void updatePlayerShots(std::vector<Ammo> &playerShots)
{
    for (auto it = playerShots.begin(); it != playerShots.end();)
    {
        // Move shot based on direction
        switch (it->direction)
        {
        case 0:
            it->y -= 2;
            break;
        case 1:
            it->x += 2;
            break;
        case 2:
            it->y += 2;
            break;
        case 3:
            it->x -= 2;
            break;
        }

        // Erase if the shot is out of bounds
        if (it->x < 0 || it->x >= WIDTH || it->y < 1 || it->y >= HEIGHT)
        {
            it = playerShots.erase(it);
        }
        else
        {
            mvprintw(it->y, it->x, "*");
            ++it;
        }
    }
}

void generateAsteroid(std::vector<Asteroid> &asteroids)
{
    Asteroid asteroid;
    int side = rand() % 4; // randomly select side (0: top, 1: right, 2: bottom, 3: left)

    switch (side)
    {
    case 0: // Top
        asteroid.x = rand() % WIDTH;
        asteroid.y = 1;               // Start above screen
        asteroid.dx = rand() % 3 - 1; // [-1, 0, 1]
        asteroid.dy = rand() % 2 + 1; // [1, 2]
        break;
    case 1:                     // Right
        asteroid.x = WIDTH - 1; // Start on right side of screen
        asteroid.y = rand() % HEIGHT;
        asteroid.dx = -(rand() % 2 + 1);
        asteroid.dy = rand() % 3 - 1;
        break;
    case 2: // Bottom
        asteroid.x = rand() % WIDTH;
        asteroid.y = HEIGHT - 1; // Start below screen
        asteroid.dx = rand() % 3 - 1;
        asteroid.dy = -(rand() % 2 + 1);
        break;
    case 3:             // Left
        asteroid.x = 0; // Start on left side of screen
        asteroid.y = rand() % HEIGHT;
        asteroid.dx = (rand() % 2 + 1);
        asteroid.dy = rand() % 3 - 1;
        break;
    }

    asteroids.push_back(asteroid);
}

void updateAsteroids(std::vector<Asteroid> &asteroids)
{
    for (auto it = asteroids.begin(); it != asteroids.end();)
    {
        it->x += it->dx;
        it->y += it->dy;

        // Erase if the asteroid is out of bounds
        if (it->x < 0 || it->x >= WIDTH || it->y < 1 || it->y >= HEIGHT)
        {
            it = asteroids.erase(it);
        }
        else
        {
            mvprintw(it->y, it->x, "@");
            ++it;
        }
    }
}

// Collision between ship and asteroids
void checkShipAsteroidCollision(Ship &ship, std::vector<Asteroid> &asteroids)
{
    for (auto itAsteroid = asteroids.begin(); itAsteroid != asteroids.end();)
    {
        // If collision, check shield/hp and decrement while destroying asteroid
        if (ship.x == itAsteroid->x && ship.y == itAsteroid->y)
        {
            if (ship.shield > 0)
            {
                ship.shield = std::max(ship.shield - 25, 0);
            }
            else
            {
                ship.health -= 25;
            }
            itAsteroid = asteroids.erase(itAsteroid);
        }
        ++itAsteroid;
    }
}

// Collision between playerShots and asteroids
void checkAmmoAsteroidCollision(std::vector<Ammo> &playerShots, std::vector<Asteroid> &asteroids)
{
    for (auto itAmmo = playerShots.begin(); itAmmo != playerShots.end();)
    {
        for (auto itAsteroid = asteroids.begin(); itAsteroid != asteroids.end();)
        {
            // If collision, destroy both shot and asteroid
            if (itAmmo->x == itAsteroid->x && itAmmo->y == itAsteroid->y)
            {
                itAmmo = playerShots.erase(itAmmo);
                itAsteroid = asteroids.erase(itAsteroid);
            }
            else
            {
                ++itAsteroid;
            }
        }
        ++itAmmo;
    }
}

void gameLoop(Ship &ship)
{

    int count = 0;

    // Set up variables for timing
    auto startTime = std::chrono::steady_clock::now();
    auto lastGameUpdateTime = startTime;
    auto lastScoreUpdateTime = startTime;
    const std::chrono::milliseconds gameUpdateInterval(100);   // 100 milliseconds
    const std::chrono::milliseconds scoreUpdateInterval(1000); // 1 second

    // Enable non-blocking input
    nodelay(stdscr, TRUE);

    while (true)
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
                generatePlayerShot(playerShots, ship);
                break;
            case 'q':
                return;
            }
        }

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
            int asteroidFrequency = std::max(10 - static_cast<int>(totalElapsedTime.count() / 1000), 5);
            if (rand() % asteroidFrequency == 0)
            {
                generateAsteroid(asteroids);
            }

            clear();

            // Update game state
            drawStatusBars(ship);
            drawShip(ship);

            updatePlayerShots(playerShots);
            updateAsteroids(asteroids);

            mvprintw(2, 0, "working after %d iters with %zu asteroids and %zu shots", count, asteroids.size(), playerShots.size());

            checkShipAsteroidCollision(ship, asteroids);
            checkAmmoAsteroidCollision(playerShots, asteroids);

            if (ship.health <= 0)
            {
                return;
            }

            refresh();

            lastGameUpdateTime = currentTime;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Delay
    }

    nodelay(stdscr, FALSE);
}

int main()
{

    Ship ship = initShip();

    initNcurses();

    // Define color pairs and set default color
    init_pair(COLOR_PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(COLOR_PAIR_DEFAULT));

    drawStatusBars(ship);
    drawShip(ship);

    gameLoop(ship);

    // End ncurses
    endwin();
    return 0;
}