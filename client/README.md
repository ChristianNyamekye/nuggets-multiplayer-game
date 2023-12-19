# Client
This directory contains the `client` code for the nuggets game. It is the code that allows the player to connect to a server to play the game. It contains both the spectator and player

Main contributer: Caleb Ash

## Compiling
To compile the program, `make` or `make client`.

To clean the directory, `make clean`.

To compile any of the programs used for unit testing, compile them individually. They don't rely on other programs.

## Usage
The player's window must be large enough to fit the contents of the map. To start the game, do the following:
```
./client hostname port [player name]
```
where player name is optional. If you want to play the game, enter a player name shorter than 50 characters. If you want to spectate, don't enter a player name.

## Testing
To fully test the program, a `server` needs to be running before a client is created. 

Test programs to test portions of the code along the way are included and can be ran simply by compiling them and seeing the output.

To run the testing script, do the following:
```
./testing.sh
```
This will run the test script. The output will go to stderr.

## Assumptions
* This program assumes that each `grid` has at most 999 columns and 99 rows.

## Notes
* Client needs a `server` to run the game
* Has memory leaks caused by `ncurses`
* When the server is busy and running slow, there can be flashes on the screen of the user. This is caused by the program running slow.