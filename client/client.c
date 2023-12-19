/*
 * Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye
 * CS50 Spring, 2023
 *
 * Client program for nuggets project
 * Handles all client-side parts of the nuggets project
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <stdbool.h>
#include <signal.h>
#include "../support/message.h"
#include "../support/log.h"
#include "../grid/grid.h"

//Function prototypes
static bool verifyArgs(const int argc, const char* argv[]);
static void initializeDisplay(int nrows, int ncols);
static void quitGame(const char* input);
static void serverComs(const char* serverHost, const char* serverPort);
static bool handleInput(void* arg);
void sendToServer(addr_t to, char* input);
static bool handleMessage(void* arg, const addr_t addr, const char* input);
static void showDisplay(void);
static void resize(int i);
void checkWinSize(void);
static void readGold(const char* input);
static void readGrid(const char* input);
static void cleanGame(void);

//Some global things to help out
static const int maxNameLength = 50;
static char player_char = '\0';
char* serverHost;
char* serverPort;
char* map;
int nrows = 0;
int ncols = 0;

//Gold numbers
int n = 0;
int p = 0;
int r = 0;

//Player message
char* displayMessage;
char* addedMessage;
char* player_name;
char* playMsg;
bool addExtra = false;
bool isSpec = false;
bool showGold = false;

addr_t* addr;


/*
 * Main function that runs the program by calling appropriate functions
 */
/********************* main *********************/
int 
main(const int argc, const char* argv[])
{
  //Verify the command line arguments before anything
  bool check = verifyArgs(argc, argv);
  if (check) {
    
    //Allocating memory for globally made char*s
    serverHost = malloc(strlen(argv[1]) + 1);
    serverPort = malloc(strlen(argv[2]) + 1);
    player_name = malloc(maxNameLength + 1);
    playMsg = malloc(strlen("PLAY ") + maxNameLength + 1);
    displayMessage = malloc(sizeof(char*) + 25);
    addedMessage = malloc(sizeof(char*) + 25);
    map = malloc(sizeof(char*) * 500 * 500);

    log_init(stderr);

    //Copy over host name and port from command line
    strcpy(serverHost, argv[1]);
    strcpy(serverPort, argv[2]);

    addr = malloc(sizeof(addr_t));

    //If 2 arguments, it's a spectator
    if (argc == 3) {
      isSpec = true;
    }
    //Else, it's a player, create play message with character to send
    if (argc == 4) {
      //addedMessage = malloc(sizeof(char*) + 25);
      strcpy(player_name, argv[3]);
      char* play = "PLAY ";
      strcpy(playMsg, play);
      strcat(playMsg, player_name);
    }

    //Call serverComs to start the process of the game
    serverComs(serverHost, serverPort);

  }

  //If command line arguments are invalid
  else {
    fprintf(stderr, "Command line arguments failed\n");
    return 1;
  }
  cleanGame();
  return 0;
}

/*
 * Function to verify the arguments of the command line
 * Returns true if arguments are valid, otherwise false
 */
/****************** verifyArgs ******************/
static bool
verifyArgs(const int argc, const char* argv[])
{
  //If incorrect number of arguments
  if (argc != 3 && argc != 4) {
    fprintf(stderr, "Error: incorrect number of command line arguments\n");
    fprintf(stderr, "Usage: ./client hostname port [playername]\n");
    return false;
  }

  //If the port is not a number
  int i;
  for (i = 0; i < strlen(argv[2]); i++) {
    if (!isdigit(argv[2][i])) { 
      fprintf(stderr, "Please input a valid port number for second argument\n");
    return false;
    }
  }

  //Checking if there is a player name, and if so, making sure name isn't too long
  if (argc == 4 && strlen(argv[3]) > maxNameLength) {
    fprintf(stderr, "Your name length is too long. Please have it be less than 50 characters\n");
    return false;
  }

  //Return true if everything passes
  return true;
}

/*
 * Function to initialize the display of the screen
 */
/************* initializeDisplay **************/
static void
initializeDisplay(int nrows, int ncols)
{
  //Initialize the screen, put into cbreak and noecho mode
  initscr();
  cbreak();
  noecho();
  
  //Setting color of the map
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  attron(COLOR_PAIR(1));

  //Refresh to update
  refresh();
}

/*
 * Function to handle quitting the game
 * Takes in input (the message from server)
 */
/**************** quitGame *******************/
static void
quitGame(const char* input)
{
  //Safety check to make sure message is passed
  if (input == NULL) {
    log_v("input to quitGame was NULL\n");
    exit(1);
  }

  log_s("%s \n", displayMessage);

  //Print out the message to the screen
  //printw("%s \n", input);

  //Shutting down game
  attroff(COLOR_PAIR(1));
  endwin();

  //Outputting the quit message
  fprintf(stdout, "%s \n", input);

  //Call cleanGame to free memory
  //cleanGame();
}

/*
 * Function that does the message protocol for client
 * Takes in char* for host name, and port
 */
/**************** serverComs ******************/
static void
serverComs(const char* serverHost, const char* serverPort)
{
  //Safety check to make sure parameters are good
  if (serverHost == NULL || serverPort == NULL) {
    log_v("Error: host name and/or port passed to serverComs are NULL\n");
    exit(1);
  }

  //Initialize message
  message_init(stderr);
  
  //Setting address using message.c function
  bool set = message_setAddr(serverHost, serverPort, addr);

  //Making sure set went well
  if (!set) {
    log_v("ERROR: In serverComs, setting address\n");
    exit(1);
  }

  //If spectator, send message to spectate game
  if (isSpec) {
    message_send(*addr, "SPECTATE");
  }
  //Else, send message to play the game
  else {
    message_send(*addr, playMsg);
  }

  //Message loop using handleInput and handleMessage to loop for messages
  bool loop = message_loop(addr, 0.0, NULL, handleInput, handleMessage);

  //If loop ends, output message
  if (!loop) {
    log_v("loop ended in serverComs do to fatal error\n");
  }
  else {
    log_v("Game ended successfully\n");
  }

  //Finishing message
  message_done();
}

/*
 * Helper function for message_loop that takes a void* arg
 * Returns false to keep the message loop going
 * Main purpose is to receive keystrokes from the user and send them to server
 */
/*************** handleInput ******************/
static bool
handleInput(void* arg)
{
  char ch = getch();
  char* m;

  //Checking if spectator pressed a bad key
  if (isSpec) {
    if (ch == 'Q') {
      m = "KEY Q";
      sendToServer(*addr, m);
    }
    else {
      log_v("Spectator can only send 'Q' \n");
    }
  }

  //If getch returns EOF, quit
  if (ch == EOF) {
    m = "KEY Q";
    sendToServer(*addr, m);
  }

  //Making m = to first part of KEY message, assigning memory for temp
  m = "KEY ";
  char* temp = malloc(strlen(m) + 2);

  //Copying over m to temp
  strcpy(temp, m);

  //adding the keystroke to the end of temp
  temp[strlen(m)] = ch;

  //Adding NULL character to end of temp
  temp[strlen(m) + 1] = '\0';
  
  //Message sending KEY message to server with check to make sure key was appended to
  if (strcmp(temp, "KEY ") != 0) {
    log_s("message sent is: %s \n", temp);
    sendToServer(*addr, temp);
  }

  free(temp);
  return false;
}

/*
 * Simple helper function to send a message to server
 * Takes an address to send to, message to send
 */
/************** sendToServer ****************/
void
sendToServer(addr_t to, char* input)
{
  if (input == NULL) {
    log_v("input is NULL in sendToServer");
  }
  //Making sure address is valid, if so, send the message passed to server
  if (message_isAddr(to)) {
    message_send(to, input);
  }
  //Otherwise print error message
  else {
    log_v("ERROR: address in sendToServer is invalid\n");
  }
}

/*
 * Helper function passed into message_loop
 * Handles all messages as per the message protocol in the requirements spec
 *
 */
/************* handleMessage *****************/
static bool
handleMessage(void* arg, const addr_t from, const char* input)
{

  if (strncmp(input, "QUIT", strlen("QUIT")) == 0) {
    quitGame(input);
    return true;
  }

  if (strncmp(input, "OK", strlen("OK")) == 0) {
    int length = strlen(input);

    //Checking to see if message is appropriate
    if (length != 4) {
      log_v("ERROR: message OK passed incorrectly in handleMessage\n");
      //free(input);
      return true;
    }

    player_char = input[3];

    //free(message);
    return false;
  }

  //Handling GRID message
  if (strncmp(input, "GRID", strlen("GRID")) == 0) {
    readGrid(input);
    checkWinSize();

    //Checking int value could be created for rows
    if (nrows == 0) {
      log_v("ERROR: nrows could not be converted to int in handleMessage\n");
    }

    //Checking int value could be created for columns
    if (ncols == 0) {
      log_v("ERROR: ncols couldn't be converted to int in handleMessage\n");
    }

    //free(message);
    return false;
  }

  //Handling GOLD message
  if (strncmp(input, "GOLD", strlen("GOLD")) == 0) {

    //Call readGold function, passing input string into parameter
    readGold(input); 
    return false;
  }

  //Handling DISPLAY message
  if (strncmp(input, "DISPLAY", strlen("DISPLAY")) == 0) {
    strcpy(map, input + strlen("DISPLAY "));
    
    showDisplay();
    return false;
  }

  //Handling ERROR Message
  if (strncmp(input, "ERROR", strlen("ERROR")) == 0) {
    
    int endLen = strlen(input) - strlen("ERROR ");
    int totalLen = strlen(input);

    //Putting message into addedMessage
    strcpy(addedMessage, input + totalLen - endLen);
    
    addExtra = true;
    showGold = false;

    //free(message);

    return false;
  }

  //If there's a bad message, return true to terminate message loop
  log_v("ERROR: Bad Message sent to the client\n");
  //free(message);
  return true;
}

/*
 * Function called in handleMessage to show the display
 * No parameters, nothing returned
 */
/**************** showDisplay *******************/
static void
showDisplay(void)
{
  //Clear screen, print display message at top
  clear();
  mvprintw(0, 0, "%s", displayMessage);

  //If extra message to be displayed, print that message after
  int len = strlen(displayMessage);
  if (addExtra) {
    mvprintw(0, len, "%s", addedMessage);
  }
  
  len = len + strlen(addedMessage);

  //Adding spaces after to fill out
  while (len < ncols) {
    addch(' ');
    move(0, len);
    len++;
  }

  //Print out the map from global variable map
  mvprintw(1, 0, "%s", map);
  //refresh();
  
  //Looping through all characters in ncurses and moving curser to '@'
  if (!isSpec) {
    move(0, 0);
    int i = 0;
    int j = 0;
    char ch;
    int found = 0;
    for (i = 0; i < nrows + 1; i++) {
      for (j = 0; j < ncols + 1; j++) {
        ch = mvinch(i, j);
        move(i, j);
        if (ch == '@') {
          found = 1;
          break;
        }
      }
      if (found) {
        break;
      }
    }
  }

  //Refresh screen
  refresh();
}

/*
 * Function to help with the resize of the window
 */
/***************** resize *****************/
static void
resize(int i)
{
  //End old screen and create new one
  endwin();
  initscr();
  noecho();
  cbreak();

  //Printing out resize messaging
  mvprintw(0, 0, "Please expand the size of your window\n");
  mvprintw(1, 0, "Press the 'return' key when ready\n");
  refresh();
}

/*
 * Function that does the work of checking window size
 */
/************** checkWinSize ****************/
void
checkWinSize(void)
{
  //Call initialize display
  initializeDisplay(nrows, ncols);
  int height;
  int width;

  getmaxyx(stdscr, height, width);
  bool checkSize = true;

  //If the screen is too small
  if (width < ncols || height < nrows) {
    while (checkSize) {
      //Signal for window resize
      signal(SIGWINCH, resize);

      getmaxyx(stdscr, height, width);
      char ch = getch();

      //If they entered resize
      if (ch == '\n') {
        if (width >= ncols && height >= nrows) {
          //Breaks the loop
          checkSize = false;
        }
      }
    }
  }


}

/*
 * Function called in handleMessage to read GOLD message
 * Takes in a char* that is the GOLD message
 * Returns nothing, handles GOLD message and updates message
 */
/***************** readGold ********************/
static void
readGold(const char* goldMessage)
{
  int i = 0;

  //Allocating memory
  char* nug = malloc(sizeof(char*) + 5);
  char* purse = malloc(sizeof(char*) + 5);
  char* rem = malloc(sizeof(char*) + 5);
  char* temp1 = malloc(strlen(goldMessage) + 5);

  int endLen;

  endLen = strlen(goldMessage) - strlen("GOLD ");

  //Copying in the end of the message to temp1
  strcpy(temp1, goldMessage + strlen(goldMessage) - endLen);

  //Iterating i until there's a space to see how long it is
  while (!isspace(temp1[i])) {
    i++;
  }

  //Adding spaces to allocated memory
  for (int t = 0; t < 10; t++) {
    purse[t] = ' ';
    rem[t] = ' ';

    if (t < i + 2) {
      nug[t] = ' ';
    }
  }
  
  //Copying over value for n to nug from temp
  strncpy(nug, temp1, i);
  nug[i+1] = '\0';

  int j = i + 1;

  //Moving over to next part for p
  while (!isspace(temp1[j])) {
    j++;
  }

  //Copying over p for purse from temp
  strncpy(purse, temp1 + 1 + i, j - i - 1);
  purse[j-i-1] = '\0';

  int k = j + 1;

  //Moving over for nuggets remaining
  while (k < strlen(temp1) && !isspace(temp1[k])) {
    k++;
  }

  //Copying over nuggets remaining to rem from temp1
  strncpy(rem, temp1 + j + 1, k - j);
  rem[k - j + 1] = '\0';

  //Add spectator stuff

  //Getting values for all three parts of message
  n = atoi(nug);
  p = atoi(purse);
  r = atoi(rem);

  //If spectator, displayMessage is only this
  if (isSpec) {
    sprintf(displayMessage, "Spectator: %d nuggets unclaimed. Play at %s %s.", r, serverHost, serverPort);
  }

  //If there are newly nuggets collected
  if (n > 0) {
    //Adding display message
    sprintf(displayMessage, "Player %c has %d nuggets (%d unclaimed).", player_char, p, r);
    //Adding this to display Message
    sprintf(addedMessage, " GOLD received: %d", n);
    addExtra = true;
    showGold = true;
  }
  //If nothing new collected
  else if (n == 0 && !isSpec) {
    sprintf(displayMessage, "Player %c has %d nuggets (%d unclaimed).", player_char, p, r);
    showGold = false;
  }

  //Freeing memory
  free(nug);
  free(purse);
  free(rem);
  free(temp1);
}

/*
 * Function to be called in handleMessage to read GRID message
 * Takes in gridMessage as paramter to be read
 * Returns nothing, handles gridMessage
 */
/***************** readGrid ********************/
static void
readGrid(const char* gridMessage)
{
  int i = 0;
  char* rows = malloc(sizeof(char*));
  char* cols = malloc(sizeof(char*));

  char* temp = malloc(strlen(gridMessage));

  //If both cols and rows are in single digits
  if (strlen(gridMessage) == 8) {
    strcpy(temp, gridMessage + strlen(gridMessage) - 3);

    //Copying in values from temp to cols and rows
    strcpy(cols, temp + strlen(temp) - 1);
    strncpy(rows, temp, 1);
  }

  //if one of cols/rows is in single digits, and other is double digits
  if (strlen(gridMessage) == 9) {
    strcpy(temp, gridMessage + strlen(gridMessage) - 4);

    while(!isspace(temp[i])) {
      i++;
    }
    
    //If the nrows is one digit, ncols is two digits
    if (i == 1) {
      //Copying values from temp to rows and cols
      strncpy(rows, temp, 1);
      strcpy(cols, temp + strlen(temp) - 2);
    }

    //If the nrows is two digits, ncols is one digit
    if (i == 2) {
      //Copying values from temp to ncols and nrows
      strncpy(rows, temp, 2);
      strcpy(cols, temp + strlen(temp) - 1);
    }
  }

  //If both cols and rows are in double digits
  if (strlen(gridMessage) == 10) {
    strcpy(temp, gridMessage + strlen(gridMessage) - 5);

    //Copying values from temp to ncols and nrows
    strcpy(cols, temp + strlen(temp) - 2);
    strncpy(rows, temp, 2);
  }

  if (strlen(gridMessage) == 11) {
    strcpy(temp, gridMessage + strlen(gridMessage) - 6);

    strncpy(rows, temp, 2);
    strcpy(cols, temp + strlen(temp) - 3);
  }
  //Converting to integers to be used
  nrows = atoi(rows);
  ncols = atoi(cols);

  //Freeing memory
  free(temp);
  free(cols);
  free(rows);
}

/*
 * Function to clean game at the end of game
 * Takes no parameters, simply frees memory
 */
/****************** cleanGame ******************/
void
cleanGame(void)
{
  if (serverHost != NULL) {
    free(serverHost);
  }

  if (serverPort != NULL) {
    free(serverPort);
  }

  if (playMsg != NULL) {
    free(playMsg);
  }

  if (!isSpec && player_name != NULL) {
    free(player_name);
  }

  if (map != NULL) {
    free(map);
  }

  if (displayMessage != NULL) {
    free(displayMessage);
  }

  if (!isSpec && addedMessage != NULL) {
    free(addedMessage);
  }

  if (addr != NULL) {
    free(addr);
  }
}