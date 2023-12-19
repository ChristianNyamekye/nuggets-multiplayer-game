/* 
 * grid.h - header file for CS50 project 'grid' module
 * 
 * A 'grid' is ...
 *
 * Bash Brigade, May 2023
 * Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye
 */

#ifndef __GRID_H
#define __GRID_H

#include "../server/server.h"
#include "file.h"

grid_t* grid_new();

void grid_insert(grid_t* grid, point_t* point, int row, int col);

point_t* grid_get(grid_t* grid, int row, int col);

int grid_getNR(grid_t* grid);

int grid_getNC(grid_t* grid);

char point_getPlayer(point_t* point);

void point_setPlayer(point_t* point, char id);

point_t* point_new(char value);

int point_getVal(point_t* point);

char point_getValue(point_t* point);

void point_setGold(point_t* point, int count);

int point_getGold(point_t* point);

void point_setTrack(point_t* point, bool status);

bool point_getTrack(point_t* point);

void point_setVal(point_t* point, int val);

grid_t* initializeMap(FILE* fp);

void generateGold(grid_t* grid, int GoldMinNumPiles, int GoldMaxNumPiles, int GoldTotal);

grid_t* visualizeMap(grid_t* grid, int PR, int PC);

void updateMap(grid_t* playerGrid, int PR, int PC);

void delete_grid(grid_t* grid);

void point_delete(point_t* point);

#endif // __GRID_H