/*
 * Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye
 * CS50 Spring, 2023
 *
 * Client program for nuggets project
 * Handles all client-side parts of the nuggets project
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "support/message.h"
#include "support/log.h"
//#include "grid/grid.c"

bool verifyArgs(int argc, char* argv[]);
static int initializeDisplay(int nrows, int ncols);

static const int MaxNameLength = 50;

typedef struct game {
  int gold_left;
  int players_count;
  player_t** players;
  grid_t* global;
} game_t;

/********************* main *********************/
int 
main(const int argc, const char* argv[])
{
  bool check = verifyArgs(argc, argv);
  if (check) {
    //Do the rest
  }
  else {

    fprintf(stderr, "Command line arguments failed\n");
    return 1;

  }

  return 0;
}

/****************** verifyArgs ******************/
bool
verifyArgs(int argc, char* argv[])
{
  if (argc != 3 || argc != 4) {
    fprintf(stderr, "Error: incorrect number of command line arguments\n");
    fprintf(stderr, "Usage: ./client hostname port [playername]\n");
    return false;
  }

  if (!isdigit(argv[2]) || argv[2] < 4000 || argv[2] > 4999) { 
    fprintf(stderr, "Please input a valid port number for second argument\n");
    return false;
  }

  //Checking if there is a player name, and if so, making sure name isn't too long
  if (argc == 4 && strlen(argv[3]) > MaxNameLength) {
    fprintf(stderr, "Your name length is too long. Please have it be less than 50 characters\n");
    return false;
  }

  return true;
}

/************* initializeDisplay **************/
static int
initializeDisplay(int nrows, int ncols)
{
  //Work with ncurses


}

//This should be called in parseServer
/**************** quitGame *******************/
static int
quitGame(char* input)
{
  if (input == NULL) {
    fprintf(stderr, "input to quitGame was NULL\n");
    return 1;
  }

  if (strcmp(input, "QUIT") == 0) {
    fprintf(stdout, "You have left the game\n"); //Do the actual quitting
    return 0;
  }
  else {
    fprintf(stderr, "Message passed to quitGame was incorrect\n");
    return 1;
  }

}

/**************** serverComs ******************/
static void
serverComs(char* serverHost, const int serverPort, char* message)
{
  if (serverHost == NULL) {
    fprintf(stderr, "Error: hostname passed to serverComs is NULL\n");
    exit(1);
  }

  addr_t addr = malloc(sizeof(addr_t));

  int port = message_init(stderr);
   
  bool set = message_setAddr(serverHost, port, addr);

  if (!set) {
    fprintf(stderr, "ERROR: In serverComs, setting address\n");
    exit(1);
  }

  message_send(addr, message); //Taking in message from parameters

  bool loop = message_loop(arg, 0.0, NULL, handleInput, NULL);

  if (!loop) {
    fprintf(stderr, "loop ended in serverComs\n");
    exit(1);
  }

  //Finishing message
  message_done();
  free(addr);
}

/*************** handleInput ******************/
static bool
handleInput(void) //void??
{
  char* word == malloc(sizeof(char*));
  scanf("%s", &word);

  free(word);
}

/*************** parseServer ******************/
char**
parseServer(char** input) 
{
  int port = message_init(stderr);

  bool set = message_setAddr()

  message_done();
}

/************* handleMessage *****************/
static bool
handleMessage(addr_t* addr, char** input) //void??
{

  //Maybe can change to case switches instead
  int i;

  if (strcmp(input[0], "QUIT") == 0) {
    int quit = quitGame(); //Check correctness
    //figure out what to return, if anything
    return false;
  }
  if (strcmp(input[0], "OK") == 0) {
    //Handle
    return false;
  }
  if (strcmp(input[0], "GRID") == 0) {
    
    //incrementing an int to the length of message
    for (i = 0; input[i] != '\0'; i++) {
      //Body is blank, purely just to iterate
    }

    //Checking if length is not 3
    if (i != 2) {
      fprintf(stderr, "ERROR: message GRID passed incorrectly in handleMessage\n");
      return true;
    }

    int nrows = atoi(input[1]);

    //Checking int value could be created for rows
    if (nrows == 0) {
      fprintf(stderr, "ERROR: nrows could not be converted to int in handleMessage\n");
    }

    int ncols = atoi(input[2]);

    //Checking int value could be created for columns
    if (ncols == 0) {
      fprintf(stderr, "ERROR: ncols couldn't be converted to int in handleMessage\n");
      return true;
    }

    //Passing int value to initializeGrid
    int grid = initializeGrid(nrows, ncols);

    return false;
  }

  if (strcmp(input[0], "GOLD") == 0) {
    for (i = 0; input[i] != '\0'; i++) {
      //Body is blank, purely to iterate
    }

    //Catching error if message is bad
    if (i != 3) {
      fprintf(stderr, "ERROR: GOLD message is bad in handleMessage\n");
      return true;
    }

    game->gold_left = atoi(input[3]);
    //Do we need a player struct within the client that will hold how muchgold they hold as well as everything like that??

    //Updating the value of gold left by subtracting n from r
    if (atoi(input[1]) > 0) {
      game->gold_left = game->gold_left - atoi(input[1]);
    }
    //Do something with purse

    return false;
  }

  if (strcmp(input[0], "DISPLAY") == 0) {
    //Handle
    return false;
  }

  if (strcmp(input[0], "ERROR") == 0) {
    for (i = 0; input[i] != '\0'; i++) {
      //Body is blank, purely to iterate
    }

    if (i == 0) {
      fprintf(stderr, "ERROR: bad ERROR message in handleMessage\n");
      return true;
    }

    int j;
    for (j = 1; input[j] != '\0'; j++) {
      //Put it to the display line status
      fprintf(stderr, "%s ", input[j]);
    }
    fprintf(stderr, "\n");


    return false;
  }

  //If there's a bad message, return true to terminate message loop
  return true;
}
