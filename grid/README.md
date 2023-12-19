# CS50 Nuggets Project
## CS50 Spring 2023

### Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye
#### Primary author of grid.c: Natasha Kapadia

### Grid

An `grid` is a module that stores information about a map.
The `grid` has an int NR that stores the number of rows in the map, an int NC that stores the number of columns in the map, and a map which is a 2D array of Points.

### Usage

The *grid* module, defined in `grid.h` and implemented in `grid.c`, implements a grid of `point_t*`, and exports the following functions:
```c
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
void updateMap(grid_t* playerGrid, int PR, int PC);
void delete_grid(grid_t* grid);
void point_delete(point_t* point);
```
### Implementation

We implement this grid as a two-dimensional array of Points with integer values to hold the number of rows and columns

The *grid* itself is represented as a `struct grid` containing an int NR, int NC, and a 2D array of points

Each point in the array is a `struct point`, a type defined internally for the game.
Each point includes a `char value`, a `char ID`, an `int goldCount`, an `int val`, a `bool visibilityTrack`, and a `bool invisibleGold`.

To insert a new point in the set we create a new point to hold the relevant information, and insert it at the given row and column.

### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - compilation procedure
* `grid.h` - the interface
* `grid.c` - the implementation
* `gridTest.c` - unit test driver

### Compilation

To compile, simply `make`.

### Testing

The `gridTest.c` was used to test the accuracy of the visibility and gold functions and to assist with debugging before integration.

To test, simply `./gridTest ../maps/main.txt`.

To test with valgrind, `valgrind --leak-check=full --show-leak-kinds=all ./gridTest ../maps/main.txt`.

### Notes

There are two compilation warnings from the grid_insert function and grid_get function because it is inserting a point_t* into an array of point_t**. Not much could be done to mitigate these warnings without compromising the functionality of other components of the program. These warnings do not affect the efficacy of the nuggets game or the grid module.




