# CS50 Nuggets
## Implementation Spec
### Bash Brigade, Spring, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes a grid module.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor
We plan on splitting the labor in the following way:
- Caleb Ash will write the client program and create the map for the team (client)
- Natasha Kapadia will write the grid module and deal with player visibility (grid/visibility)
- Emi Rohn will write the player module and also deal with spectators, as well as working on other server tasks(server)
- Christian Nyamekye will deal with server communication in the server program, and other miscellaneous server requirements (server)

We will individually test our programs and modules, and each person who is responsible for their module  will create the Makefile for it. We are collectively responsible for documentation.

## Client

### Data structures
- We make use of the `addr` structure which is described in the message module provided to us.

### Function Prototypes
`client` makes use of the following function prototypes:
```c
static bool verifyArgs(const int argc, const char* argv[]);
static void initializeDisplay(int nrows, int ncols);
static void quitGame(const char* input);
static void serverComs(const char* serverHost, const char* serverPort);
static bool handleInput(void* arg);
static void sendToServer(addr_t to, char* input);
static void handleMessage(void* arg, const addr_t from, const char* input);
static void showDisplay(void);
static void resize(int i);
void checkWinSize(void);
static void readGold(const char* goldMessage);
static void readGrid(const char* gridMessage);
void cleanGame(void);
```

### Definition of function prototypes

A function which verifies the command line arguments, outputs error message if there’s an error. `verifyArgs` also checks whether the client has entered in spectator mode or as a player. If they entered as a player, it passes along the player name.
```c
static bool verifyArgs(const int argc, const char* argv[]);
```

`initializeDisplay` is a function that takes in an int for nrows and ncols, and simply starts up ncurses functions.
```c
static void initializeDisplay(int nrows, int ncols);
```

`quitGame` exits the game and prints out an end of game summary upon sending *Q* to server
```c
static void quitGame(const char* input);
```

`serverComs` does the grand scheme message protocol for the client.
```c
static void serverComs(const char* serverHost, const char* serverPort);
```

`handleInput` is a helper function passed into `message_loop` within `serverComs`. It takes in input from the user using `getch` from `ncurses`, and sends it to the server.
```c
static bool handleInput(void* arg);
```

`sendToServer` is a helper function for `handleInput`, which checks to see if the address and message sent to the server are good. If so, it sends the message to the server.
```c
static void sendToServer(addr_t to, char* input);
```

`handleMessage` is a helper function passed into `message_loop` in `serverComs`. It reads messages from the server, parsing them and taking appropriate action depending on what is sent.
```c
static void handleMessage(void* arg, const addr_t from, const char* input);
```

`showDisplay` is a helper function called within `handleMessage` that displays both the player message at the top and displays the map, as well as updating the curser so it follows the player.
```c
static void showDisplay(void);
```

`resize` is a helper function to help resize instructions. It simply restarts ncurses, and prints out the instructions for resizing. It is called within `checkWinSize`. The *int* parameter simply passes through and is not used.
```c
static void resize(int i);
```

`checkWinSize` is a helper function called within `handleMessage` to check the window size. It calls `resize` if the window is too small, it loops until the user’s window size is large enough and they press “return” on the keyboard.
```c
void checkWinSize(void);
```

`readGold` is a helper function called within `handleMessage`. It is used to read a `GOLD` message from the server. It takes in the message, breaks it up into integers `n`, `p`, and `r` for use.
```c
static void readGold(const char* goldMessage);
```

`readGrid` is a helper function called within `handleMessage`. It is used to read a `GRID` message from the server. It takes the message, and breaks it up into integers `nrows` and `ncols` for use.
```c
static void readGrid(const char* gridMessage);
```

`cleanGame` is a function called within `quitGame` that frees all the memory associated with the client (other than ncurses related memory).
```c
void cleanGame(void);
```


### Detailed pseudo code

#### `parseArgs`:

	validate commandline
	initialize message module
	print assigned port number
	decide whether spectator or player

#### `main`:
```
Call verifyArgs to verify command line arguments
If verified:
Allocate memory for everything necessary
Initialize log
Determine whether spectator or player
Create the correct play message
Call serverComs
Else:
	Print error to stderr
```

#### `verifyArgs`:
```
if argc is not 3 or 4
	Print error message, exit non-zero
if argv[2] is not a number
	Print error message, exit non-zero
if argv[3] exists and is a name that is too long 
	Print error message, exit non-zero
```

#### `initializeDisplay`:
```
Call initial ncurses functions
refresh
```

#### `quitGame`:
```
Log the display message (the quit message)
Turn off ncurses
Output quit message to stdout
Call cleanGame to free memory
```

#### `serverComs`:
```
Initialize message module
Set the address of the host
If spectator:
	Send spectate message to server
If player:
	Send play message to host
Message_loop with handleInput, handleMessage
Log results
message_done
```

#### `handleInput`:
```
Assign a character ch to getch to listen to user input
If spectator:
	If ch equals ‘Q’
		Send Q to server
	Else:
		Log that spectator can only send a ‘Q’
If ch equals EOF:
	Send ‘Q’ to server
Copy over whatever keystroke ch is to a KEY message
Log keystroke
Send KEY message to server
Return false to keep message_loop going
```

#### `sendToServer`:
```
Check address and message sent
If good, call message_send to send to server
```

#### `handleMessage`:
```
If input message is QUIT:
	Call quitGame to exit
	Return true to break message loop
If input message is OK:
	Check that the message is valid
	If not valid:
		Log the error
		Return true to stop loop and stop game
	Assign player character
	Return false to continue loop
If input message is GRID:
	Call readGrid
	Call checkWinSize
	Check that nrows and ncols were assigned properly
	Return false to continue loop
If input message is GOLD:
	Call readGold
	Return false to continue loop
If input message is DISPLAY:
	Copy string to map string in client
	Call showDisplay
	Return false to continue loop
If input message is ERROR:
	Copy error message to addedMessage
	Make sure you are displaying addedMessage
	Return false to continue loop
If any other message:
	Log bad message
	Return true to kill loop
```

#### `showDisplay`:
```
Clear screen
Print the display message at the top, including added message if called for
Print out the map below the display message
If not a spectator:
	Set curser to the top left
	For each point in the map (display):
		Check if the character there is an ‘@’
		When the ‘@’ is found:
			Move the curser to that spot
			Break the loop(s)
Refresh
```

#### `resize`:
```
End current window
Restart ncurses functions
Print out instructions for resizing
```

#### `checkWinSize`:
```
Call initializeDisplay
Check the size of the user’s window
If it’s too small:
	Create loop for while it’s too small
	While in loop:
		Call signal (included in signal.h) with resize
		Recheck size
		If the user pressed “return”:
			If the window size is now large enough:
				Break loop
```

#### `readGold`:
```
Allocate memory for any necessary strings
Iterate through string until reaching a space
When reach first space, take that value as n
Iterate until another space
When reach second space, take that value as p
Iterate until another space
When reach final space, take that value as r
If spectator:
	Create display message for spectator
If n is greater than zero:
	Create player message for gold
	Create added message for gold received
Else if n equals zero and is player:
	Create only the standard player message
```

#### `readGrid`:
```
Allocate necessary memory for strings
If both columns and rows are single digits:
	Copy over correctly to nrows and ncols
If one of either columns and rows are double digits and the other is single digits:
	Check if either rows or columns are double digits
	Copy over correctly to nrows and ncols
If both columns and rows are double digits:
	Copy over to nrows and ncols accordingly
If ncols is triple digits and nrows is double digits:
	Copy over to nrows and ncols accordingly
Convert strings for nrows and ncols to integers
```

#### `cleanGame`:
```
Free all memory associated with the game, with checks on each free to make sure it’s necessary.
```

---

## Server
### Implementation Spec for Server
## Primary authors: Christian Nyamekye & Emiko Rohn

### Internal Structure
*server* includes the following libraries and files:
```c
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>        // socket-related calls
#include <sys/select.h>       // select-related stuff 
#include "server.h"
#include "../grid/file.h"
#include "../support/message.h"
#include "../support/log.h"
#include "../grid/grid.h"

```

*server* makes use of the following internal functions:
```c
static game_t* new_game(char* mapFileName);
static int game_setup(game_t* game);
void parseArgs(const int argc, char* argv[], char** mapFileName, char** optionalSeed);
static player_t* new_player(char* fullname, game_t* game, addr_t addr);
static bool assign_random_spot(player_t* player, game_t* game);
static char* to_string(game_t* game, addr_t from);
static bool move_player(game_t* game, player_t* player, int currX, int currY, int newX, int newY);
static void remove_player(player_t* player, game_t* game);
static void read_message(game_t* game, player_t* player, char* message);
static void assign_player_gold(player_t* player, game_t* game);
static player_t* id_to_player(game_t* game, char id);
static void spectate(game_t* game, addr_t address);
static int validate_seed(char* optionalSeed);
static player_t* addr_to_player(game_t* game, addr_t addr);
static bool handle_message(void* arg, const addr_t from, const char* message);
static void leave_game(game_t* game);
static void delete_game(game_t* game);

```
*server* defines the following constants:
```c
static const int goldTotal = 250;      // amount of gold in the game
static int goldCollected = 0;      // amount of gold in the game
static const int goldMinNumPiles = 10; // minimum number of gold piles
static const int goldMaxNumPiles = 30; // maximum number of gold piles
static const int maxPlayers = 26;      // maximum number of players
```
*server* uses the following structs:
```
struct coordinate{
    int x;
    int y;
};

typedef struct point{
    char* value;
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

```
### Internal Modules and Functions

#### main
`main` is responsible for validating arguments, initializing other methods and modules, and freeing the memory associated with `game`.

##### Pseudocode
```c
int main(int argc, char *argv[]) {
    Initialize log module
    parseArgs();validates number of arguments, mapFilename, and optional seed    Validate filename
    Initialize game with new_game()
    game_setup()
    leave_game()
    Free memory allocated in main()
    log_done()
    exit 0
}
```

### Definition of function prototypes

`initialize_player` creates a new player object, given the full name of a player from the client. 
```c
player_t* new_player(const char* fullname, game_t* game, addr_t addr);
```

`new_game` function creates a new game object that contains the global grid, array of players, and amount of gold. 
```c
game_t* new_game(char* makeFileName);
```

A function which finds a valid random place in the grid and assigns the player to that spot. Returns true of the spot was added correctly. 
```c
bool assign_random_spot(player_t* player, game_t* game);
```

A function that takes in a players address, and converts their respective grid to string form to be sent to the client. 
```c
char* to_string(game_t* game, addr_t from);
```

A function that takes a player’s ID, iterates through the game, and returns the player object. 
```c
static player_t* id_to_player(game_t* game, char id);
```

A function that takes an address and a game, and returns the player object that matches the address. 
```c
static player_t* addr_to_player(game_t* game, addr_t addr);
```

A function that removes an individual player from the game. 
```c
static void remove_player(player_t* player, game_t* game);
```

A function that allows a spectator to watch the game from a specific address. 
```c
static void spectate(game_t* game, addr_t address);
```

A function to delete the game and remove all players. 
```c
static void delete_game(game_t* game);
```

A function which validates `mapFilename` and `optionalSeed`.
```c
void parseArgs(const int argc, char* argv[], char** mapFileName, char** optionalSeed);
```

A function that allocates memory and initializes the game struct. It initializes the server and listens for clients 
```c 
static int game_setup(game_t* game);
```

A function that takes in a message from a client and acts accordingly. It quits when *q*  and calls *move_player* otherwise
```c
static void read_message(game_t* game, player_t* player, char* message);
```

A function that handles command from clients. Commands could be *PLAY*, *KEY*, OR *SPECTATE* 
```c
static bool handle_message(void* arg, const addr_t from, const char* message);
```

A function that confirms  that seed is a positive integer
```c
static int validate_seed(char* optionalSeed);
```

A function that allows an individual player to leave the game and provides game summary.
```c
static void leave_game(game_t* game);

```
A function that updates/changes the positions of clients on the grid.
```c
static bool move_player(game_t* game, player_t* player, int currX, int currY, int newX, int newY);
```

### Detailed pseudo code

#### parseArgs
`parseArgs` validates that argument number is either two or three and initializes them to a variable.
##### Pseudocode
```c
void
parseArgs(const int argc, char* argv[], char** mapFileName, char** optionalSeed){
    Validates number of arguments
    Validates map.txt file; checks to see if it’s valid
    Runs validat_seed() to confirm optional seed
}
```


#### validate_seed
`validate_seed` confirms that the seed provided is a positive integer greater than zero and then converts the string to an `int` and returns it to the caller.

##### Pseudocode

#### new_game
```c
Allocate space for a new game
Create an array to hold the players
Iterate through the array and set every player initially to NULL
Initialize all game-related variables to be zero or NULL
Initialize the game’s mapfile to be the one passed as a parameter
Generate gold on the map
Return the game
```

#### new_player
```c
If the game count hasn’t been exceeded
	Allocate space for a player
	If the address is valid
		Assign it to the player’s port
	Otherwise
		Give the player no address
	Set the players name
	Set the purse and just collected to 0
	Convert the alphabet to the id using the player count
	Initially, set the player’s grid to NULL
	Open the mapfile and initialize the player’s grid
	Assign the random spot to a player
	Update the map based on visibility
	Set the player in the global map
	Add the player to the game’s array
	Increment the player count
	Return the player
Else
	Send a message that max number has been received
```

#### assign_random_spot
```c
Get the NR and NC for this grid
While spot has not been assigned
	Generate a random x and y val within NR and NC respectively
	Get the point at those coordinates
	If the spot is valid
		Move the player there
		Update the global map
		Break the loop
If the spot was assigned
	Return true
Else
	Return false
```

#### to_string
```c
Assign the player to the address given
Initially, the player is false
If the player exists
	The local grid is the player grid
Otherwise
	The spectator is true
Iterate through every row
	Iterate through every column
		If it’s not a spectator
			If the point is visible
				If a player doesn’t exist at the point
					If the gold is invisible at that spot
						Add the value to the string
					Otherwise if there is gold
						Add the “*”
				Otherwise
					Add the value to the string
				Otherwise if a player exists there
					If the player is us
						Add the “@”
					If the player is different than us
						Add the player’s ID
			Otherwise
				Add an empty space
		Else if it’s a spectator
			If the point is visible
				If a player doesn’t exist at the point
					If the gold is invisible at that spot
						Add the value to the string
					Otherwise if there is gold
						Add the “*”
				Otherwise
					Add the value to the string
				Otherwise if a player exists there
					Add the player’s ID
	Add a new line characters
Return the string
```

#### id_to_player
```c
Iterate through every player in the game’s array
	If the player’s ID is equal to the passed ID
		Return that player
Otherwise return NULL
```

#### addr_to_player
```c
Iterate through every player in the game’s array
	If the player’s address is equal to the passed address
		Return that player
Otherwise return NULL
```

#### validate_seed
```c
static int validate_seed(char *seedChar) {
    Confirm all characters in string are digits
    atoi()
}
```

#### game_setup
`game_setup` preps the game to run - reading in a game struct and generating gold piles. Returns non-zero status in the case of an error.

##### Pseudocode
```c
static int game_setup(game_t* game) {
    Initialize the server and port 
    message_init(NULL)
    Listen for messages from clients
    message_loop()
    Returns 0
}
```

#### handle_message
`handle_message` is the function used by `message_loop()` to handle messages from clients. It will initialize `new_game()`, `read_message()`, or `spectate()` based on the message type recieved from the client. It then updates all clients with changes in the game environment. Function returns `true` in order to break the loop if all gold has been collected or there’s no player in-game. Otherwise returns `false`.

##### Pseudocode
```c
static bool handle_message(void* arg, addr_t from, const char* message) {
    If client sent a PLAY message
        new_game()
    If client sent a KEY message
        read_message()
    If client sent a SPECTATE message
        spectate()
    Update all players with updatePlayers()
    Update spectator with GOLD and DISPLAY message
    If there is no more gold to collect
        return true
    Otherwise
        return false
}
```

#### move_player
`move_player` either swaps with a another player or change position of player and updates map.

##### Pseudocode
```c
static bool move_player(game_t* game, player_t* player, int currX, int currY, int newX, int newY) {
    If a player is there
        SWAP the two players
	   Globally Set the old player to the old coordinates
        Locally Set the current player to the new coordinates
	   Locally Remove the current player from the old coordinates

    If there’s no at that spot
        If it is a room or a passage
		Move the player there
			Collect the gold if there’s one
			Update the point to have no gold left
			Remove player from old spot and Update map
	   Return true
    Else
	  Update map and return false
}
```

#### read_message
`read_message` sends a QUIT message to the client if appropriate. Otherwise, it calls `move_player` and updates player position if necessary.

##### Pseudocode
```c
static void read_message(game_t* game, player_t* player, char* message) {
    If key is Q or q
        Send QUIT message
    Otherwise if the client is a spectator
        Ignore message. Return
   Act on appropriate input message
    move_player()
    
}
```

#### spectate
```c
If the current spectators address is equal to the old one
	Send them a quit message to kick them off
Otherwise
	Set the old spectator to be the current one
Send the grid message by getting the NR and NC for this game
Send the gold message by getting the game’s amount of gold left
Send the display message by calling to_string
Free the messages just sent
```

#### leave_game
`leave_game` compiles the end of game rankings table and broadcasts it to players.

##### Pseudocode for `leaveGame`
```c
static void leave_game(game_t* game) {
    Create QUIT table header
    Iterate of game->player_array
    For each player in the array
        Add a line to the table with their letter, purse, and name
    Broadcast QUIT table to all players
    Clean up
    Send QUIT table message to spectator
}
```

#### remove_player
```c
Delete the player’s grid
Iterate through the player’s array in the game
	Set this player to point to NULL
Free the player name
Free the player
Decrement the player count
```

#### delete_game
```c
For every player in the player array
	If the player isn’t NULL
		Remove them
Delete the grid
Free the game’s player array
Free the game
```

---

## Grid

### Data structures
```c
typedef struct point{
    char* value;
    char id;
    int goldCount;
    int val;
    bool visibilityTrack;
    bool invisibleGold;
} point_t;
```

- The *grid* has an int NR, an int NC, and a map which is a 2D array of Points

### Definition of function prototypes
A function that initializes a new grid
```c
grid_t* grid_new();
```

A function that inserts a point into a grid at row and col
```c
void grid_insert(grid_t* grid, point_t* point, int row, int col)
```

A function that returns the point at row and col in the grid
```c
point_t* grid_get(grid_t* grid, int row, int col);
```

A function that gets the number of rows in a grid
```c
int grid_getNR(grid_t* grid);
```

A function that gets the number of columns in a grid
```c
int grid_getNC(grid_t* grid);
```

A function that sets the number of rows in a grid
```c
void grid_setNR(grid_t* grid, int NR);
```

A function that sets the number of columns in a grid
```c
void grid_setNC(grid_t* grid, int NC);
```

A function that gets the player ID from a point
```c
char point_getPlayer(point_t* point);
```

A function that sets the player ID of a point
```c
void point_setPlayer(point_t* point, char id);
```

A function that gets the integer value of a point
```c
int point_getVal(point_t* point);
```

A function that sets the goldCount of a point
```c
void point_setGold(point_t* point, int count);
```

A function that returns the goldCount of a point
```c
int point_getGold(point_t* point);
```

A function that sets the visibilityTrack of a point
```c
void point_setTrack(point_t* point, bool status);
```

A function that returns the value of a point
```c
char* point_getValue(point_t* point)
```

A function that returns the visibilityTrack of a point
```c
bool point_getTrack(point_t* point);
```

A function that sets the integer value of a point
```c
void point_setVal(point_t* point, int val);
```

A function that initializes and returns a new point
```c
point_t* point_new(void* item)
```

A function that takes a mapFile and initializes a grid module which is an array of arrays of points
```c
grid_t* initializeMap(char* mapFileName);
```

A function that randomly allocates and inserts gold into the map
```c
void generateGold(grid_t* grid, int GoldMinNumPiles, int GoldMaxNumPiles, int GoldTotal);
```

A function that updates the map in accordance with a player’s move
```c
void updateMap(grid_t* playerGrid, int PR, int PC);
```

A function that deletes a point
```c
void point_delete(point_t* point);
```

A function that deletes a grid
```c
void delete_grid(grid_t* grid);
```


### Detailed pseudo code

 #### grid_insert
```
insert the point into the grid’s map at row and col
```

 #### grid_get
```
return the point in the grid’s map at row and col
```

 #### grid_getNR
```
return the grid’s NR value
```

 #### grid_getNC
```
return the grid’s NC value
```

 #### grid_setNR
```
if grid is not null
set the grid’s NR value
```

 #### grid_getNC
```
if grid is not null
set the grid’s NC value
```

 #### point_getPlayer
```
if point is not null
	return point’s ID value
else return a space
```

 #### point_setPlayer
```
if point is not null
	set point’s ID value
```

 #### point_getVal
```
if point is not null
	return point’s integer value
else return 0
```

 #### point_setGold
```
if point is not null
	set point’s goldCount
```

 #### point_getGold
```
if point is not null
	return point’s goldCount
else return 0
```

 #### point_setTrack
```
if point is not null
	set visibilityTrack to given status
```

 #### point_getValue
```
if point is not null
	return point’s value
else return null
```

 #### point_getTrack
```
if point is not null
	return point’s visibilityTrack
else return null
```

 #### point_setVal
```
if point is not null
	set point’s val
```

 #### point_new
```
allocate memory for the point
if value is null
	return null 
else if the point is null
	return null
else 
	set point’s value
	initialize id to blank space
	initialize visibilityTrack to false
	initialize invisibleGold to false
	return point
```

 #### initializeMap
```
create a new grid
open mapFile for reading
for each row in the map
	read the line
	for each character in the line
		read character at that position and initialize corresponding struct
		initialize Point to store information about that character
		add Point to the grid
set NR and NC
return the grid
```

#### generateGold
```	
randomly select a number of piles between GoldMinNumPiles and GoldMaxNumPiles
for each pile
	generate a random number of nuggets between number of piles left and gold left
	generate two random coordinates for each pile
	get the Point corresponding to those coordinates
	while Point is not a room spot
		generate two random coordinates for each pile
		get the Point corresponding to those coordinates
	add gold to that Point
insert Point back into map
```

#### updateMap
```
get playerPoint at player’s new coordinates
if point is a passage, make visible and make immediate passage neighbors visible
for each currPoint in the map
	if currPoint is already visible and has no gold
		continue
	if currPoint is a space or a passage
		continue
	if currPoint is in the same row as playerPoint
for each testPoint in between the two points
stop if testPoint will block visibility
	else if currPoint and playerPoint are in adjacent rows
		for each column in between them
			get Point in both rows at that column
stop if Point will block visibility
	else
		for each row in between currPoint and playerPoint
			determine column value where the line would intersect that row
			if column is an integer
				get testPoint at the row, column
				stop if testPoint will block visibility
			if column is not an integer
				get point1 at row, column (rounded down)
				stop if point1 will block visibility
get point2 at row, column (rounded up)
				stop if point2 will block visibility
			if stop is true
				break 
		if stop is false
			set currPoint to visible
			insert currPoint back into grid
		else if there is gold but stop is true
			set currPoint to invisibleGold
			insert currPoint back into grid	
```

 #### point_delete
```
if point is not null
	free the point
```

 #### delete_grid
```
for each row in the grid
	for each column in the grid
		get point at row, column
		delete point
free the grid
```

---

## Testing plan

### unit testing

#### Grid

The `gridTest.c` was used to test the accuracy of the visibility and gold functions and to assist with debugging before integration.

#### Client
There are a few programs used for client testing: `testreadgrid.c`, `testerror.c`, `test_gold.c`, and `messagesend.c`. Additionally, `testing.sh` is a test script meant to test incorrect client command line inputs. All outputs from that test script go to `stderr`.

#### Server
All unit testing are done with the provide miniclient and unit testing of other modules. Additionally, `testing.sh` is a test script meant to test incorrect server command line inputs. 

### integration testing

#### Client
Client integration testing is done using the provided miniserver.c. This test that the functions for message sending and connecting to a server are working correctly. To test graphics and things like that, a full working server is needed.

#### Server
Server integration testing is done using the provided miniclient.c. This test that the functions for message sending and connecting to a server are working correctly. All other functions are tested with the provided client in the shared directory by running a variation of the code below.

```c
 ~/cs50-dev/shared/nuggets/linux/client babylon5 35977 christian 2> ~/cs50-dev/nuggets2
-bash-brigade/client/clientlog.txt
```


### system testing

System testing involves the comprehensive examination of all three components: the Server, Grid, and Client, to ensure their seamless integration and proper functioning together. Logging is utilized as a troubleshooting mechanism during the testing process.

In addition, the `testing.sh` script is employed to test the exit values of the system and compare them against the expected values. This ensures that the system successfully reaches the desired state, denoted by an exit value of 0.

Furthermore, the `server.c` code is specifically tested with the `bash-brigade.txt` file to verify the smooth execution of the compiled code with the designated map.

To assess the robustness of the code, various edge cases are tested. These include scenarios such as logging in 26 clients, accommodating 2 spectators, and evaluating the system's response to premature exits and invalid command-line inputs. These tests help ensure that the code can handle diverse and challenging situations effectively.

---

## Limitations
* If the server is running slow, the screen will flash for the clients
* For the client, there are lots of memory errors lost. They are ALL ncurses related and not created from our program.
* Grid compiles with two warnings. They do not affect the game and are due to the design of the program

---
## Extra Credit
We did four of the options for extra credit:
1. Optional server arguments for `--gold 500`, `--minpiles 15`, and `--maxpiles 40`.
2. Allowing players to steal gold from each other by stealing from them
3. An additional optional server argument `--plain`, which reverts the gane back to its normal version
4. When a player quits, they leave a gold pile of all their gold where they last where before they quit

It was implemented in the following ways: