# Programming Project 2 - Celestial Planet Protector

## Overview

This project is a survival game where you fly a ship in outer space and attempt to get a high score.

## Description

- The ship is controlled with arrow keys or WASD, and space to fire shots
- Asteroids and alien ships fly through the environment causing damage
- There are shield and ammo boosts, points are earned for destroying enemies

## Implementation

- User input is handled for movement, firing, and quitting
- Uses a game state with vectors to handle positions and movements of objects
- Coordinates are constantly checked for collisions

## Bugs/Issues

- Sometimes collision detection does not work as expected due to input being updated at a faster rate than collision detection

## Usage

- Compile and run with 'make' and '/game'
