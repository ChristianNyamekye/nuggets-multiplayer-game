/* 
 * server.h - header file for CS50 project 'server' module
 *
 * Bash Brigade, May 2023
 * Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye
 */

#ifndef __SERVER_H
#define __SERVER_H

#include "../support/message.h"

typedef struct point{
    char value;
    char id;
    int goldCount;
    int val; // 0 for empty, 1 for room, 2 for horizontal wall, 3 for passage, 4 for corner, 5 for vertical wall
    bool visibilityTrack;
    bool invisibleGold;
} point_t;

typedef struct grid{
    int NR;
    int NC;
    point_t** map[500][500];
} grid_t;

typedef struct player{
    char* name;
    char id;
    grid_t* grid;
    int xPos;
    int yPos;
    int justCollected;
    int purse;
    addr_t port;
}player_t;

typedef struct game{
    grid_t* global;
    player_t** players_array;
    char* mapFileName;
    int goldLeft;
    int goldCollected;
    int playerCount;
    int quitCount;
    addr_t spectator;
}game_t;

/* 
 * Takes in a pointer to a string filename of a map txt file. 
 * Can also take in an optional seed, but not necessary.
*/
void parseArgs(const int argc, char* argv[], char** mapFileName, char** optionalSeed);

#endif // __SERVER_H