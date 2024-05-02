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

GameState::GameState()
{
    // Constructor
}

void GameState::printInfo()
{
    mvprintw(2, 0, "%zu asteroids, %zu shots, and %zu aliens", asteroids.size(), playerShots.size(), aliens.size());
}

void GameState::generatePlayerShot(Ship &ship)
{
    // Check if ship has enough ammo
    if (ship.ammo > 0)
    {
        ship.ammo--;
        PlayerAmmo shot1;
        PlayerAmmo shot2;
        shot1.direction = ship.direction;
        shot2.direction = ship.direction;
        shot1.x = ship.x;
        shot1.y = ship.y;
        //(0: up, 1: right, 2: down, 3: left)
        if (ship.direction == 0)
        {
            shot2.x = ship.x;
            shot2.y = ship.y - 1;
        }
        else if (ship.direction == 1)
        {
            shot2.x = ship.x + 1;
            shot2.y = ship.y;
        }
        else if (ship.direction == 2)
        {
            shot2.x = ship.x;
            shot2.y = ship.y + 1;
        }
        else
        {
            shot2.x = ship.x - 1;
            shot2.y = ship.y;
        }

        playerShots.push_back(shot1);
        playerShots.push_back(shot2);
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
        // asteroid.dy = rand() % 2 + 1; // [1, 2]
        asteroid.dy = 1;
        break;
    case 1:                     // Right
        asteroid.x = WIDTH - 1; // Start on right side of screen
        asteroid.y = rand() % HEIGHT;
        // asteroid.dx = -(rand() % 2 + 1);
        asteroid.dx = -1;
        asteroid.dy = rand() % 3 - 1;
        break;
    case 2: // Bottom
        asteroid.x = rand() % WIDTH;
        asteroid.y = HEIGHT - 1; // Start below screen
        asteroid.dx = rand() % 3 - 1;
        // asteroid.dy = -(rand() % 2 + 1);
        asteroid.dy = -1;
        break;
    case 3:             // Left
        asteroid.x = 0; // Start on left side of screen
        asteroid.y = rand() % HEIGHT;
        // asteroid.dx = (rand() % 2 + 1);
        asteroid.dx = 1;
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

void GameState::generateAlien()
{
    if (aliens.size() < 5)
    {
        Alien alien;
        alien.fireCooldown = 5;
        int side = rand() % 4; // randomly select side (0: top, 1: right, 2: bottom, 3: left)

        switch (side)
        {
        case 0: // Top
            alien.x = rand() % WIDTH;
            alien.y = 1; // Start above screen
            break;
        case 1:                  // Right
            alien.x = WIDTH - 1; // Start on right side of screen
            alien.y = rand() % HEIGHT;
            break;
        case 2: // Bottom
            alien.x = rand() % WIDTH;
            alien.y = HEIGHT - 1; // Start below screen
            break;
        case 3:          // Left
            alien.x = 0; // Start on left side of screen
            alien.y = rand() % HEIGHT;
            break;
        }

        aliens.push_back(alien);
    }
    else
    {
        return;
    }
}

void GameState::updateAliens(Ship &ship)
{
    for (auto it = aliens.begin(); it != aliens.end();)
    {
        // Calculate direction towards player ship
        int dx = ship.x - it->x;
        int dy = ship.y - it->y;

        // Put into unit vector
        int length = std::max(std::abs(dx), std::abs(dy));
        if (length != 0)
        {
            dx /= length;
            dy /= length;
        }

        bool collision = false;
        for (auto other = aliens.begin(); other != aliens.end(); ++other)
        {
            if (other != it && other->x == it->x + dx && other->y == it->y + dy)
            {
                collision = true;
                break;
            }
        }

        // If no collision, move
        if (!collision)
        {
            it->x += dx;
            it->y += dy;
            // mvprintw(it->y, it->x, "O");
            ++it;
        }
        else
        {
            // Try random directions
            if (rand() % 2 == 0)
            {
                if (rand() % 2 == 0)
                {
                    it->x += (rand() % 2 == 0) ? -1 : 1; // left or right
                }
                else
                {
                    it->y += (rand() % 2 == 0) ? -1 : 1; // up or down
                }
            }
            else
            {
                it->y += (rand() % 2 == 0) ? -1 : 1;
            }
            // mvprintw(it->y, it->x, "O");
            ++it;
        }
    }
}

void GameState::drawAliens()
{
    for (auto it = aliens.begin(); it != aliens.end();)
    {
        mvprintw(it->y, it->x, "O");
        ++it;
    }
}

void GameState::generateAlienShot(Ship &ship)
{
    for (auto it = aliens.begin(); it != aliens.end();)
    {
        if (it->fireCooldown <= 0)
        {
            // Calculate direction towards player ship
            int dx = ship.x - it->x;
            int dy = ship.y - it->y;

            // Put into unit vector
            int length = std::max(std::abs(dx), std::abs(dy));
            if (length != 0)
            {
                dx /= length;
                dy /= length;
            }

            AlienAmmo shot = {
                it->y,
                it->x,
                dy, dx};
            alienShots.push_back(shot);

            it->fireCooldown = 50;
            ++it;
        }
        else
        {
            it->fireCooldown--;
            ++it;
        }
    }
}

void GameState::updateAlienShots()
{
    for (auto it = alienShots.begin(); it != alienShots.end();)
    {
        it->x += it->dx;
        it->y += it->dy;
        mvprintw(it->y, it->x, ".");
        ++it;
    }
}

// Collision between ship and asteroids
void GameState::checkShipAsteroidCollision(Ship &ship)
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
            // Exit loop since asteroid is destroyed
            // break;
        }
        else
        {
            ++itAsteroid;
        }
    }
}

// Collision between playerShots and asteroids
void GameState::checkPlayerAmmoAsteroidCollision()
{
    for (auto itAmmo = playerShots.begin(); itAmmo != playerShots.end();)
    {
        bool collision = false;
        for (auto itAsteroid = asteroids.begin(); itAsteroid != asteroids.end();)
        {
            // If collision, destroy both shot and asteroid
            if (itAmmo->x == itAsteroid->x && itAmmo->y == itAsteroid->y)
            {
                itAmmo = playerShots.erase(itAmmo);
                itAsteroid = asteroids.erase(itAsteroid);
                collision = true;
                break;
            }
            else
            {
                ++itAsteroid;
            }
        }
        if (!collision)
        {
            ++itAmmo;
        }
    }
}

// Collision between player ship and alien ships
bool GameState::checkShipAlienCollision(Ship &ship)
{
    for (auto itAlien = aliens.begin(); itAlien != aliens.end();)
    {
        if (ship.x == itAlien->x && ship.y == itAlien->y)
        {
            return false;
        }
        else
        {
            ++itAlien;
        }
    }
    return true;
}

// Collision between ship and alien shots
void GameState::checkShipAlienAmmoCollision(Ship &ship)
{
    for (auto itAlienShot = alienShots.begin(); itAlienShot != alienShots.end();)
    {
        // If collision, check shield/hp and decrement while destroying asteroid
        if (ship.x == itAlienShot->x && ship.y == itAlienShot->y)
        {
            if (ship.shield > 0)
            {
                ship.shield = std::max(ship.shield - 25, 0);
            }
            else
            {
                ship.health -= 25;
            }
            itAlienShot = alienShots.erase(itAlienShot);
        }
        else
        {
            ++itAlienShot;
        }
    }
}

// Collision between alien ships and player shots
void GameState::checkAlienPlayerAmmoCollision()
{
    for (auto itAmmo = playerShots.begin(); itAmmo != playerShots.end();)
    {
        bool collision = false;
        for (auto itAlien = aliens.begin(); itAlien != aliens.end();)
        {
            // If collision, destroy both shot and asteroid
            if (itAmmo->x == itAlien->x && itAmmo->y == itAlien->y)
            {
                itAmmo = playerShots.erase(itAmmo);
                itAlien = aliens.erase(itAlien);
                collision = true;
                break;
            }
            else
            {
                ++itAlien;
            }
        }
        if (!collision)
        {
            ++itAmmo;
        }
    }
}

// Collision between alien ships and asteroids
void GameState::checkAlienAsteroidCollision()
{
    for (auto itAlien = aliens.begin(); itAlien != aliens.end();)
    {
        bool collision = false;
        for (auto itAsteroid = asteroids.begin(); itAsteroid != asteroids.end();)
        {
            // If collision, destroy both shot and asteroid
            if (itAlien->x == itAsteroid->x && itAlien->y == itAsteroid->y)
            {
                itAlien = aliens.erase(itAlien);
                itAsteroid = asteroids.erase(itAsteroid);
                collision = true;
                break;
            }
            else
            {
                ++itAsteroid;
            }
        }
        if (!collision)
        {
            ++itAlien;
        }
    }
}