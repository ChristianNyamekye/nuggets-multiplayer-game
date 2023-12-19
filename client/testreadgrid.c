#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static void readGrid(char* gridMessage);

int main()
{
  char* msg1 = "GRID 2 5";
  char* msg2 = "GRID 12 5";
  char* msg3 = "GRID 8 19";
  char* msg4 = "GRID 32 12";
  
  readGrid(msg1);
  readGrid(msg2);
  readGrid(msg3);
  readGrid(msg4);

  return 0;
}


static void
readGrid(char* gridMessage)
{
  int i = 0;
  char* rows = malloc(sizeof(char*));
  char* cols = malloc(sizeof(char*));

  char* temp = malloc(strlen(gridMessage));

  //If both cols and rows are in single digits
  if (strlen(gridMessage) == 8) {
    strcpy(temp, gridMessage + strlen(gridMessage) - 3);

    strcpy(cols, temp + strlen(temp) - 1);
    strncpy(rows, temp, 1);
  }

  //if one of cols/rows is in single digits, and other is double digits
  if (strlen(gridMessage) == 9) {
    strcpy(temp, gridMessage + strlen(gridMessage) - 4);

    while(!isspace(temp[i])) {
      i++;
    }

    if (i == 1) {
      strncpy(rows, temp, 1);
      strcpy(cols, temp + strlen(temp) - 2);
    }

    if (i == 2) {
      strncpy(rows, temp, 2);
      strcpy(cols, temp + strlen(temp) - 1);
    }
  }

  //If both cols and rows are in double digits
  if (strlen(gridMessage) == 10) {
    strcpy(temp, gridMessage + strlen(gridMessage) - 5);

    strcpy(cols, temp + strlen(temp) - 2);
    strncpy(rows, temp, 2);
  }

  int nrows = atoi(rows);
  int ncols = atoi(cols);

  //Printing out to make sure it was properly assigned
  printf("rows value is %d and cols value is %d \n", nrows, ncols);

  free(temp);
  free(cols);
  free(rows);
}