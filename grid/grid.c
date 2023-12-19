/*
* grid.c - CS50 'grid' module for final project
*
* see grid.h for more information.
*
* Bash Brigade, May 2023
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "file.h"
#include "grid.h"


/**************** grid_new ****************/
/* Allocate and initialize a grid */
grid_t* grid_new(){
 grid_t* grid = malloc(sizeof(grid_t));


 if (grid == NULL){
   return NULL;
 }
 else {
   return grid;
 }
}


/**************** grid_insert ****************/
/* insert a point into the map */
void grid_insert(grid_t* grid, point_t* point, int row, int col){
 grid->map[row][col] = point;
}


/**************** grid_get ****************/
/* return a point from the map */
point_t* grid_get(grid_t* grid, int row, int col){
 return grid->map[row][col];
}


/**************** grid_getNR ****************/
/* return NR value */
int grid_getNR(grid_t* grid){
 return grid->NR;
}


/**************** grid_getNC ****************/
/* return NC value */
int grid_getNC(grid_t* grid){
 return grid->NC;
}


/**************** grid_setNR ****************/
/* set NR value */
void grid_setNR(grid_t* grid, int NR){
 if (grid != NULL){
   grid->NR = NR;
 }
}


/**************** grid_setNC ****************/
/* set NR value */
void grid_setNC(grid_t* grid, int NC){
 if (grid != NULL){
   grid->NC = NC;
 }
}


/**************** point_getPlayer ****************/
char point_getPlayer(point_t* point){
 if(point != NULL){
   return point->id;
 }
 return ' ';
}
/**************** point_setPlayer ****************/
void point_setPlayer(point_t* point, char id){
 if(point != NULL){
   point->id = id;
 }
}


/**************** point_getVal ****************/
int point_getVal(point_t* point){
 if (point != NULL){
   return point->val;
 }
 return 0;
}


/**************** point_setGold ****************/
void point_setGold(point_t* point, int count){
 if (point != NULL){
     point->goldCount = count;
 }
 }


/**************** point_getGold ****************/
int point_getGold(point_t* point){
 if (point != NULL){
   return point->goldCount;
 }
 return 0;
}


/**************** point_setTrack ****************/
void point_setTrack(point_t* point, bool status){
 if (point != NULL){
     point->visibilityTrack = status;
 }
}


/**************** point_getValue ****************/
char point_getValue(point_t* point){
 if (point != NULL){
   return point->value;
 }else{
   return '\0';
 }
}


/**************** point_getTrack ****************/
bool point_getTrack(point_t* point){
 if (point != NULL){
   return point->visibilityTrack;
 }else{
   return NULL;
 }
}


/**************** point_setVal****************/
void point_setVal(point_t* point, int val){
 if (point != NULL){
     point->val = val;
 }
}


/**************** point_new ****************/
/* Allocate and initialize a point */
point_t*  // not visible outside this file MAYBE
point_new(char value)
{
 point_t* point = malloc(sizeof(point_t));


 if (value == ' '){
   return NULL;
 }


 else if (point == NULL) {
   // error allocating memory for node; return error
   return NULL;
 } else {
   point->value = value;
   // by default, initialize playerID to NULL, goldCount to 0, visibilityTrack to false
   point->id = ' ';
   point->goldCount = 0;
   point->visibilityTrack = false;
   point->invisibleGold = false;
   return point;
 }


 return NULL;
}


/**************** initializeMap ****************/
 grid_t* initializeMap(FILE* fp){


   // initialize modules
   char* line;
   int NR = 0;
   int NC = 0;
   int NCtrack = 0;


   // create new grid
   grid_t* grid = grid_new();


   // open and process the map input
   NR = file_numLines(fp); // get number of rows


   // iterate over map file
   for (int i = 0; i < NR; i ++){
       line = file_readLine(fp); // account for error
       char* word = line;
       while (*word != '\0'){


           // store character value
            point_t* point = point_new(word[0]); // initialize new point


           // if character is space, assign value 0
           if(*word == ' '){
             point_setVal(point, 0);
           }


           // if character is room spot, assign value 1
           else if(*word == '.'){
             point_setVal(point, 1);
           }


           // if character is a horizontal wall, assign value 2
           else if(*word == '-'){
             point_setVal(point, 2);
           }


           // if character is a passageway, assign value 3
           else if(*word == '#'){
             point_setVal(point, 3);
           }


           // if character is a corner, assign value 4
           else if(*word == '+'){
             point_setVal(point, 4);
           }


           // if character is a vertical wall, assign value 5
           else if(*word == '|'){
             point_setVal(point, 5);
           }
           grid_insert(grid, point, i, NCtrack); // put point into map
            // increment at each column loop
           NCtrack ++;
           word++;
       }


       if (NC == 0){ // set number of columns after reading first line
           NC = NCtrack;
       }
         
       // increment at each room loop
       NCtrack = 0;
       free(line);
   }


   grid_setNR(grid, NR);
   grid_setNC(grid, NC);
   return grid;
}


/**************** generateGold ****************/
void generateGold(grid_t* grid, int GoldMinNumPiles, int GoldMaxNumPiles, int GoldTotal){
  // initialize key modules
 srand(time(NULL));
 int numPiles = (rand() % (GoldMaxNumPiles - GoldMinNumPiles)) + GoldMinNumPiles; // generate random number of piles within range
 int pileTrack = numPiles;
 int goldLeft = GoldTotal;
 int NR = grid_getNR(grid);
 int NC = grid_getNC(grid);


 // for each pile
 for (int i = 0; i < numPiles; i ++){


   // set number of nuggets
   int nuggets;


   // if on last pile, put all remaining gold in last pile
   if ((i + 1) == numPiles){
     nuggets = goldLeft;
   }
  
   // otherwise, generate a random number of nuggets between the number of piles left and the amount of gold left
   else {
     if (goldLeft == 1){
       nuggets = 1;
     }
     else{
       nuggets = (rand() % (goldLeft - pileTrack)) + 1;
     }
   }
  
   // put the nuggets into a random location on the map
   bool invalidSpot = true;
   int PR;
   int PC;


   while (invalidSpot){ // as long as spot is not valid
     PR = (rand() % (NR - 1)); // generate random row coordinate between 0 and NR-1
     PC = (rand() % (NC - 1)); // generate random column coordinate between 0 and NC-1
    
     point_t* point = grid_get(grid, PR, PC); // get point at these coordinates
     int value = point_getVal(point); // get value of these points
     int isGold = point_getGold(point);


     if (value == 1 && isGold == 0){ // if valid spot, assign gold and break loop
         point_setGold(point, nuggets); // set gold count to nuggets
         grid_insert(grid, point, PR, PC); // update grid with updated point
         invalidSpot = false; // break loop
     }
   }


   goldLeft = goldLeft - nuggets; // update remaining gold count
   pileTrack--;


  
   if (goldLeft == 0){ // break the loop if no more nuggets left to distribute
     break;
   }
 }
 }


/**************** updateMap ****************/
void updateMap(grid_t* playerGrid, int PR, int PC){


 // initialize key variables
 int NR = grid_getNR(playerGrid);
 int NC = grid_getNC(playerGrid);
 point_t* playerPoint = grid_get(playerGrid, PR, PC);


 // if point is a passage, make visible and make immediate neighbors visible
 int check = point_getVal(playerPoint);
 if (check == 3){
     point_setTrack(playerPoint, true);
     grid_insert(playerGrid, playerPoint, PR, PC);


     point_t* above = grid_get(playerGrid, PR-1, PC);
     if (point_getVal(above) == 3){
       point_setTrack(above, true);
       grid_insert(playerGrid, above, PR-1, PC);
     }


     point_t* below = grid_get(playerGrid, PR+1, PC);
     if (point_getVal(below) == 3){
       point_setTrack(below, true);
       grid_insert(playerGrid, below, PR+1, PC);
     }


     point_t* left = grid_get(playerGrid, PR, PC-1);
     if (point_getVal(left) == 3){
       point_setTrack(left, true);
       grid_insert(playerGrid, left, PR, PC-1);
     }


     point_t* right = grid_get(playerGrid, PR, PC+1);
     if (point_getVal(right) == 3){
       point_setTrack(right, true);
       grid_insert(playerGrid, right, PR, PC+1);
     }
       
   }


 // iterate over all points in map and determine if each is visible to the player
 for (int r = 0; r < NR; r++){
   for (int c = 0; c < NC; c++){


     // get point at row r and column c
     point_t* currPoint = grid_get(playerGrid, r, c);
   
     // if a point is already visible and has no gold, it stays visible
     bool check = point_getTrack(currPoint);
     int goldCheck = point_getGold(currPoint);
     if (check && goldCheck == 0){
       grid_insert(playerGrid, currPoint, r, c);
       continue;
     }


     // if point is a space, no need to check
     int check2 = point_getVal(currPoint);
     if (check2 == 0){
       continue;
     }
    
     // otherwise determine if you can see point from current spot
     int rowDiff = abs(PR - r);
     int colDiff = abs(PC - c);
     bool stop = false;
     int firstRow;
     int lastRow;
     int firstCol;
     int lastCol;


     // if the points are in the same row -- just iterate through all the points in between them
     if (PR == r){
       if (PC > c){
         firstCol = c + 1;
         lastCol = PC;
       }
       else {
         firstCol = PC + 1;
         lastCol = c;
       }
      
       // for each point
       for (int i = firstCol; i < lastCol; i++){
         point_t* testPoint = grid_get(playerGrid, r, i); // get point
         int testVal = point_getVal(testPoint); // get point's value
         if (testVal == 0 ||testVal == 2 || testVal == 3 || testVal  == 4 || testVal == 5){ // if point will block
           stop = true;
           continue; // break the for loop
         }


       }
     }


     // else if the points are in neighboring rows
       else if (rowDiff >= 1){
       if (PC > c){
         firstCol = c + 1;
         lastCol = PC;
       }
       else {
         firstCol = PC + 1;
         lastCol = c;
       }
      
       // for each point
       for (int i = firstCol; i < lastCol; i++){
         point_t* testPoint = grid_get(playerGrid, r, i); // get point
         int testVal = point_getVal(testPoint); // get point's value
         if (testVal == 0 || testVal == 3 || testVal == 4){ // if point will block, note you can still see adjacent walls and corners
           stop = true;
           continue; // break the for loop
         }
         point_t* testPoint2 = grid_get(playerGrid, PR, i); // get point
         int testVal2 = point_getVal(testPoint2); // get point's value
         if (testVal2 == 0 || testVal2 == 3 || testVal2 == 4){ // if point will block, note you can still see adjacent walls and corners
           stop = true;
           continue; // break the for loop
         }
       }
      
     }


     // iterate through the rows between them
     if (rowDiff > 1){
       if (PR > r){
         firstRow = r + 1;
         lastRow = PR;
       }
       else {
         firstRow = PR + 1;
         lastRow = r;
       }
      
       int count = 0; // variable to keep track of current position


       // iterate through all the rows in between the two points
       for (int i = firstRow; i < lastRow; i ++){


         count++;
         double col;
      
         // determine column point in this line
         if (colDiff  == 0){
           col = c;
         }
         else{
           double slope = (double)colDiff/(double)rowDiff;
           if (PC > c){
             col = (slope * count) + c;
           }
           else if (PC < c){
             col = (slope * count) + PC;
           }
         }


         // if col is an integer value
         if (roundf(col) == col){
           point_t* testPoint = grid_get(playerGrid, i, roundf(col)); // get point
           int testVal = point_getVal(testPoint); // get point's value
           if (testVal == 0 ||testVal == 2 || testVal == 3 || testVal  == 4 || testVal == 5){ // if point will block
             stop = true;
             continue; // break the for loop
           }
         }
        
         // if col is not an integer
         else {
           int down = floor(col); // calculate column to the left
           int up = ceil(col); // calculate column to the right


           point_t* point1 = grid_get(playerGrid, i, down); // get point to the left
           int val1 = point_getVal(point1); // get point's value
           if (val1 == 0 || val1 == 2 || val1 == 3 || val1  == 4 || val1 == 5){ // if point will block
             stop = true;
             continue; // break the for loop
          }


           point_t* point2 = grid_get(playerGrid, i, up); // get point to the right
           int val2 = point_getVal(point2); // get point's value
           if (val2 == 0 || val2 == 2 || val2 == 3 || val2  == 4 || val2 == 5){ // if point will block
             stop = true;
             break; // break the for loop
           }


         }
         // if stop is true
         if (stop){
           break; // break out of big for loop
         }
       }
     }
     if (stop == false){ // if point is visible after iteration
       point_setTrack(currPoint, true); // set currPoint to visible
       if (goldCheck > 0){
         currPoint->invisibleGold = false;
       }
       grid_insert(playerGrid, currPoint, r, c); // insert point into playerMap
     }
     else if (check && goldCheck > 0){
         currPoint->invisibleGold = true; // set tracker to true
         grid_insert(playerGrid, currPoint, r, c); // insert point into playerMap
     }
   }
 }
}




/**************** point_delete ****************/
void point_delete(point_t* point){
 if(point != NULL){
   free(point);
 }
}


/**************** delete_grid ****************/
void delete_grid(grid_t* grid){
 for(int i = 0; i < grid->NR; i++){
   for(int j = 0; j < grid->NC; j++){
     point_t* point = grid_get(grid, i, j);
     point_delete(point);
   }
 }
 free(grid);
}

