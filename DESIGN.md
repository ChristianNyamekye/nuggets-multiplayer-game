# CS50 Nuggets
## Design Spec
### Bash Brigade, 23S, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes grid and game modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.
## Client

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI. It is best described there.

### Inputs and outputs
The client has no role in tracking gold, determining visibility, handling moves, or even checking the validity of user's keystrokes. It only passes keystrokes to the server and displays the maps that come back.

#### Inputs
The user has to input “PLAY” in order to start the game after entering command line inputs, and then the game will start. The user should input the following controls to play the game:
- `h` to move left on the screen
- `l` to move right, if possible
- `j` to move up on the screen, if possible.
- `k` to move down on the screen, if possible.
- `Q` to quit the game.
- `y` to move diagonally up and left on screen, if possible.
- `u` to move diagonally up and right on screen, if possible
- `b` to move diagonally down and left on screen, if possible.
- `n` to move diagonally down and right on screen, if possible.
- what is *possible* in this case means that the adjacent gridpoint is an empty spot, pile of gold, or another player.
- For each character for player movement, its corresponding capitalized character will automatically repeat the movement for the character until it is no longer possible.

#### Outputs
The output consists of a `grid` that acts as the playable map taking up most of the display, but only showing the viewable area for that player. The game status is at the top of the screen. The game status for a player will look something like this:
```
Player A has 23 nuggets (227 unclaimed).
```
If there is other information that needs to be displayed, it will show up next to the game status like this:
```
Player A has 23 nuggets (227 unclaimed). GOLD received: 12.
Player A has 23 nuggets (227 unclaimed). Unknown keystrokes.
```
A spectators output would consist of a view of the entire map and all players, as well as a game display that looks like this:
```
Spectator: 227 nuggets unclaimed. Play at [hostname] [port].
```

#### Logging
We log messages between the client and server and log them to stderr. We use the given log module to do so.

### Functional decomposition into modules
The `client` is broken down into several different functions or modules. They are listed here:
- *verifyArgs*, which verifies the command line arguments, outputs error message if there’s an error. `verifyArgs` also checks whether the client has entered in spectator mode or as a player. If they entered as a player, it passes along the player name.
- *initializeDisplay*, which initializes the display of the map and also checks to make sure the display window is large enough.
- *quitGame*, which, when `Q` is sent to the server, exits the game and prints out an end of game summary
- *serverComs*, which passes along any keystrokes from the user to the server. `serverComs` makes use of several helper functions that do lots of the messaging work. These are described in the implementation spec.
- *cleanGame*, which when called, frees all the memory related to the game.
 
### Pseudo code for logic/algorithmic flow
#### verifyArgs
```
if argc is not 3 or 4
	Print error message, exit non-zero
if argv[2] is not a number
	Print error message, exit non-zero
if argv[3] exists and is a name that is too long 
	Print error message, exit non-zero
```

#### initializeDisplay
```
Initializes all necessary ncurses functions
Refresh
```

#### quitGame
```
Takes in quit message passed in parameters
Logs the display message
Turns off ncurses 
Outputs the quit message to user
Calls `cleanGame`
```

#### serverComs
```
Calls `message_init`
Calls `set` from the message module
Calls `message_send` to send a message to start the game
Loops with `message_loop`, calling helper functions described in the implementation spec
Calls `message_done`
```

---
## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.
### Inputs and outputs
#### Inputs
Accepts inbound  messages from clients (players or spectators).
#### Outputs 
- Implements all game logic.
- Updates all clients whenever any player, quits, moves or gold is collected.
- When number of gold nuggets reaches zero, prepares a tabular summary (below), send a QUIT message (to all clients) with that summary, print the summary, and exit.
```
QUIT GAME OVER:
A       4 Natasha
B       16 Christian
C       177 Emi
D       53 Caleb
```
### Functional decomposition into modules

We anticipate the following modules or functions specifically with respect to the player:
*new_game*, which initializes a new game struct.
*game_setup* which initializes the server,listens for messages, and handles them.
*parseArgs* which validates inputs
*new_player* which creates and initializes a new player structs and places it in the game 
*assign_random_spot*, which finds a valid place in the grid and assigns the player to that spot
*move_player*, which attempts to move a player by checking if its movement is valid
*read_message* which accepts a message(key) and update the grid and game.
*to_string*, which collects player input/information and converts it into a string.
*remove_player*, which deletes the player struct in the game array
*spectator*, which allows a client to view the positions and gold of the entire game without participating.
*id_to_player*, returns a player given an Id
*validate_seed* which acts as input validation for `optional seed`
*leave_game*, which controls exit from the game
*addr_to_player* which returns a player given its port number.
*handle_message* which has the functionality of handling `PLAY`, `KEY`, and `SPECTATE` messages from clients.

### Pseudo code for logic/algorithmic flow

The server will run as follows:
```
execute from a command line per the requirement spec
parse the command line, validate parameters
call `new_game()` to set up data structures
Run `game_setup()` which listens and connects to the clients and
print the port number on which we wait
call message_loop(), to await clients which
initializes `handle_message` module

call leave_game() to inform all clients the game has ended
clean up
```
#### new_game
```
create a new gamer data structure, given a mapFilename
initialize game parameters 
Returns a game struct 	
```
#### parseArgs
```
Checks number of arguments
Validates mapFilename
Validates optional seed	
```
#### game_setup
```
Initializes the server
Listen for messages from clients
Passes *handle_message* to *message_loop*	
```


#### assign_random_spot
```
while coordinates have not been assigned
generate a random x-coordinate between 0 and NR
generate a random y-coordinate between 0 and NC
if the spot is valid (is a “.”)
	update the player data structure to include the new spot
	update the boolean to exit the loop
else
	continue
```
#### read_message
```
Accepts a game struct, player struct, and message 
performs appropriate actions on message.
Calls *move_player* when movement is required

```

#### move_player
```
Take in game struct, player struct, and movementcoordinates
If that movement is valid:
	Move the player
	Check if gold exists
	If entering room
Update visibility as needed through grid functions
If moving through room with corner
	Update visibility through different grid functions
If in passageway
	Add “#” accordingly
Otherwise
	Send back same version of the map
```


#### to_string
```
If player movement interferes with other player
	If there is room left/right, swap positions horizontally
	Else if there is room up/down, swap positions vertically
	Update map of other player and global map
```

#### spectator 
```
If the address matches a player in the game
	grid = player’s grid 
else: 
	Spectator = true
If spectator is false
Loop through coordinates in provided grid
Perform appropriate operations based on whether coordinate is a room, path, wall, corner or empty space.
else 
	Update spectator accordingly
```

#### handle_message
 ```	
All the ugly work of managing messages received from clients;
Parse message
If key is *play*
	Call *new_player*
Else if key is *key*
	Call *read_message*
Else if key is *spectator*
	Call *spectate*
Break loop if no more gold or all player have left
```
	 
* Note the server never explicitly exits or closes the socket, but the socket will be closed when the program is killed or terminates. *    

#### addr_to_player
```
Player getter given an addr struct
```

#### validate_seed
```
Validates *optionalSeed*
```

#### id_to_player
```
Player getter given player id
```
#### remove_player
```
frees / cleaning up
```
#### leave_game
```
Send summary message about game
Iterate through every player
	Delete each player and its contents
Delete the grid
Free the game
```

### Major data structures
The server contains a *_point_*, *_grid_*,*_game_*, and *_player_* data structures, which controls the overall game functionality. 

#### Player struct
Contains each individual player’s version of the grid struct. It also contains the x-coordinate and y-coordinate position of the player within the grid, as well as the amount of gold collected by the player. 

#### Grid struct
Contains each number of gold and number of columns of the grid and holds a `point**` map.

#### Game struct 
Contains the entire properties of the game: 
 ```
    grid_t* global;
    player_t** players_array;
    int goldLeft;
    int goldCollected;
    int playerCount;
    int quitCount;
    addr_t spectator;
```
 

#### Point struct 
Contains the properties of structures in the room. Properties may be an entrance, a door, empty space, room spot, or corner. Point also controls visibility of these properties to players.

## Grid module

### Functional decomposition
We anticipate the following modules or functions
1. *initializeMap*, will take a map file and initialize and return a map module by initializing a struct Point and corresponding coordinates for each gridpoint in the map file
2. *generateGold*, which randomly selects a number of piles between GoldMinNumPiles and GoldMaxNumPiles, randomly divides GoldTotal nuggets amongst those piles, and then randomly inserts those piles into the map
3. *updateMap*, which updates the map of a relevant player or spectator according to a player’s movements 

### Pseudo code for logic/algorithmic flow

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
	for each testPoint that could block visibility between playerPoint and currPoint
		if Point is a wall, corner, passage, or empty space
			set stop equal to True
			continue
	if stop is false
		set currPoint to visible
		insert currPoint back into grid
	else if there is gold but stop is true
		set currPoint to invisibleGold
		insert currPoint back into grid	
```

### Major data structures

The *point* is a struct that holds information about character at that position, the ID of the player at that position (if any), the amount of gold at that position (if any), the visibility of the point (true/false boolean)

The *grid* has an int NR, an int NC, and a map which is a 2D array of Points
