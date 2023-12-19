/*
* gridTest.c - CS50 'gridTest' module for final project
*
* used for hard-coded testing of grid module before integration and for debugging purposes
*
* Bash Brigade, May 2023
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "file.h"
#include "grid.h"

static void grid_print(grid_t* playerGrid, int PR, int PC);

int main(const int argc, char* argv[]){

 // initialize map
 char* mapFileName = argv[1];
 FILE* fp = fopen(mapFileName, "r");
 grid_t* playerGrid = initializeMap(fp);
 fclose(fp);
  // insert gold into the map
 generateGold(playerGrid, 10, 30, 250); 

 // test visibility by moving player around grid and printing visibility to stdout
 // note: hard-coded for optimal testing on main.txt.
 updateMap(playerGrid, 3, 12);
 grid_print(playerGrid, 3, 12);


 updateMap(playerGrid, 4, 12);
 grid_print(playerGrid, 4, 12);
  updateMap(playerGrid, 5, 12);
 grid_print(playerGrid, 5, 12);


 updateMap(playerGrid, 6, 12);
 grid_print(playerGrid, 6, 12);


 updateMap(playerGrid, 7, 12);
 grid_print(playerGrid, 7, 12);
  updateMap(playerGrid, 7, 13);
 grid_print(playerGrid, 7, 13);


 updateMap(playerGrid, 7, 14);
 grid_print(playerGrid, 7, 14);
  updateMap(playerGrid, 7, 15);
 grid_print(playerGrid, 7, 15);
  updateMap(playerGrid, 7, 16);
 grid_print(playerGrid, 7, 16);
  updateMap(playerGrid, 7, 17);
 grid_print(playerGrid, 7, 17);
  updateMap(playerGrid, 7, 18);
 grid_print(playerGrid, 7, 18);
  updateMap(playerGrid, 7, 19);
 grid_print(playerGrid, 7, 19);
  updateMap(playerGrid, 7, 20);
 grid_print(playerGrid, 7, 20);
  updateMap(playerGrid, 7, 21);
 grid_print(playerGrid, 7, 21);
  updateMap(playerGrid, 7, 22);
 grid_print(playerGrid, 7, 22);
  updateMap(playerGrid, 7, 23);
 grid_print(playerGrid, 7, 23);
  updateMap(playerGrid, 7, 24);
 grid_print(playerGrid, 7, 24);
  updateMap(playerGrid, 7, 25);
 grid_print(playerGrid, 7, 25);
  updateMap(playerGrid, 7, 26);
 grid_print(playerGrid, 7, 26);
  updateMap(playerGrid, 7, 27);
 grid_print(playerGrid, 7, 27);
  updateMap(playerGrid, 7, 28);
 grid_print(playerGrid, 7, 28);
  updateMap(playerGrid, 7, 29);
 grid_print(playerGrid, 7, 29);
  updateMap(playerGrid, 7, 30);
 grid_print(playerGrid, 7, 30);
  updateMap(playerGrid, 7, 31);
 grid_print(playerGrid, 7, 31);
  updateMap(playerGrid, 7, 32);
 grid_print(playerGrid, 7, 32);
  updateMap(playerGrid, 7, 33);
 grid_print(playerGrid, 7, 33);
  updateMap(playerGrid, 7, 34);
 grid_print(playerGrid, 7, 34);
  updateMap(playerGrid, 7, 35);
 grid_print(playerGrid, 7, 35);
  updateMap(playerGrid, 7, 36);
 grid_print(playerGrid, 7, 36);
  updateMap(playerGrid, 7, 37);
 grid_print(playerGrid, 7, 37);
  updateMap(playerGrid, 7, 38);
 grid_print(playerGrid, 7, 38);
  updateMap(playerGrid, 7, 39);
 grid_print(playerGrid, 7, 39);
  updateMap(playerGrid, 7, 40);
 grid_print(playerGrid, 7, 40);
  updateMap(playerGrid, 8, 40);
 grid_print(playerGrid, 8, 40);
updateMap(playerGrid, 9, 40);
grid_print(playerGrid, 9, 40);
updateMap(playerGrid, 9, 41);
 grid_print(playerGrid, 9, 41);
  updateMap(playerGrid, 9, 42);
 grid_print(playerGrid, 9, 42);
  updateMap(playerGrid, 9, 43);
 grid_print(playerGrid, 9, 43);
  updateMap(playerGrid, 9, 44);
 grid_print(playerGrid, 9, 44);


 // delete grid -- should be no memory leaks
 delete_grid(playerGrid);


 // exit 0
 exit(0);
}


static void grid_print(grid_t* playerGrid, int PR, int PC){
  
   fprintf(stdout, "Testing visualize: \n");
   int NR = grid_getNR(playerGrid);
   int NC = grid_getNC(playerGrid);


   // iterate over each point
   for (int i = 0; i < NR; i++){
     for (int j = 0; j < NC; j++){
         if (i == PR && j == PC){ // check if player's current position
           fprintf(stdout, "@");
           continue;
         }
         point_t* test = grid_get(playerGrid, i, j); // get point at i, j
         bool yes = point_getTrack(test);
         if (yes){ // if point visible
           int gold = point_getGold(test);
           bool invisibleGold = test->invisibleGold;
           if (invisibleGold){ // if gold is invisible
             fprintf(stdout, "%c", test->value); // print value
           }
           else if (gold > 0){ // if gold is visible and exists
             fprintf(stdout, "*"); // print *
           }
           else { // otherwise print value
             fprintf(stdout, "%c", test->value);
           }
          
         }
         else{ // if point not visible
           fprintf(stdout, " "); // print space
         }
     }
     fprintf(stdout, "\n"); // newline at the end of each row
   }
}

