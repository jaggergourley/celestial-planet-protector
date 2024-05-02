#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ncurses.h>

#include "fight.h"

// Collision between ship and asteroids
void checkShipAsteroidCollision(Ship &ship, GameState &gameState)
{
    for (auto itAsteroid = gameState.asteroids.begin(); itAsteroid != gameState.asteroids.end();)
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
            itAsteroid = gameState.asteroids.erase(itAsteroid);
        }
        ++itAsteroid;
    }
}

// Collision between playerShots and asteroids
void checkAmmoAsteroidCollision(GameState &gameState)
{
    for (auto itAmmo = gameState.playerShots.begin(); itAmmo != gameState.playerShots.end();)
    {
        for (auto itAsteroid = gameState.asteroids.begin(); itAsteroid != gameState.asteroids.end();)
        {
            // If collision, destroy both shot and asteroid
            if (itAmmo->x == itAsteroid->x && itAmmo->y == itAsteroid->y)
            {
                itAmmo = gameState.playerShots.erase(itAmmo);
                itAsteroid = gameState.asteroids.erase(itAsteroid);
            }
            else
            {
                ++itAsteroid;
            }
        }
        ++itAmmo;
    }
}