#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ncurses.h>

#include "game_state.h"
#include "game.h"

#define HEIGHT 24
#define WIDTH 80

GameState::GameState()
{
    // Constructor
}

void GameState::generatePlayerShot(Ship &ship)
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

void GameState::updatePlayerShots()
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

void GameState::generateAsteroid()
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

void GameState::updateAsteroids()
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