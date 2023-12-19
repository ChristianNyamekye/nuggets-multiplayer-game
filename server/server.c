/* 
 * server.c - CS50 'server' module
 *
 * see server.h for more information.
 *
 * Bash Brigade, May 2023
 * Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye
 */

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

static game_t* new_game(char* mapFileName);
static int game_setup(game_t* game);
void parseArgs(const int argc, char* argv[], char** mapFileName, char** optionalSeed);
static player_t* new_player(char* fullname, game_t* game, addr_t addr);
static bool assign_random_spot(player_t* player, game_t* game);
static char* to_string(game_t* game, addr_t from);
static bool move_player(game_t* game, player_t* player, int currX, int currY, int newX, int newY);
static void remove_player(player_t* player, game_t* game);
static void assign_player_gold(player_t* player, game_t* game);
static void read_message(game_t* game, player_t* player, char* message);
static player_t* id_to_player(game_t* game, char id);
static void spectate(game_t* game, addr_t address);
static bool handle_message(void* arg, addr_t from, const char* message);
static int validate_seed(char* optionalSeed);
static player_t* addr_to_player(game_t* game, addr_t addr);
static void leave_game(game_t* game);
static void delete_game(game_t* game);

/**************** Constants ****************/
static const int goldTotal = 250;      // amount of gold in the game
static int goldCollected = 0;      // amount of gold in the game
static const int goldMinNumPiles = 10; // minimum number of gold piles
static const int goldMaxNumPiles = 30; // maximum number of gold piles
static const int maxPlayers = 26;      // maximum number of players

/**************** main ****************/
int 
main(int argc, char *argv[]){

    char* mapFileName = NULL;
    char* optionalSeed = NULL;
    
    log_init(stderr);

    parseArgs(argc, argv, &mapFileName, &optionalSeed);
    
    log_v("Initialize new game from main.");
    game_t* game = new_game(mapFileName);
    
    log_v("Run game_setup from main");
    if(game != NULL){
        log_v("Game has been created\n");
        log_done();
    }

    game_setup(game);

    log_v("Run leave_game sequence\n");
    leave_game(game);

    // Clean Up
    log_v("Free mapFileName, delete game, and close log (in main)\n");
    free(mapFileName);
    free(optionalSeed);
    delete_game(game);
    log_done();

    return 0;
}

/**************** parseArgs() ****************/
void
parseArgs(const int argc, char* argv[], char** mapFileName, char** optionalSeed) {
   
   // parse arguments
   if (argc < 2 || argc > 3) {
        log_v("Invalid number of parameters provided.");
        log_done();
        exit(1);
    }

    // Validate map.txt
    *mapFileName = calloc(1, strlen(argv[1]) + 1);
    strcpy(*mapFileName, argv[1]);

    FILE* mapFile = fopen(*mapFileName, "r");
    if (mapFile == NULL) {
        log_v("Invalid map file provided. Unable to open for reading.");
        log_done();
        exit(1);
    }
    fclose(mapFile);

   // Validate seed
    if (argc == 3) {
        log_s("Seed from argv: %s", argv[2]);
        *optionalSeed = calloc(1, strlen(argv[2] + 3));
        strcpy(*optionalSeed, argv[2]);
        log_s("Seed from calloc'd string: %s", *optionalSeed);

        unsigned int seed = validate_seed(*optionalSeed);
        log_d("Seed int (in main): %d", seed);


        if (seed == -1) {
            log_s("'%s' is an invalid seed.", *optionalSeed);
            log_done();
            exit(1);
        }

        free(*optionalSeed);

        srand(seed);
        log_v("srand() called");

    }
    else { 
        log_v("Running getpid()");
        srand(getpid()); 
    }
}

/****************** new_game ******************/
static game_t* 
new_game(char* mapFileName){

    log_v("Running new_game.");

    // Allocate memory for the game
    game_t* game = malloc(sizeof(game_t));

    // If game could be properly created
    if(game != NULL){

        // Create an array holding all players
        player_t** players_array = malloc(sizeof(player_t) * maxPlayers);
        game->players_array = players_array;

        for(int i = 0; i < 27; i++){
            game->players_array[i] = NULL;
        }

        game->playerCount = 0;    // Count used to keep track of which number player we're on
        game->quitCount = 0;
        game->goldLeft = goldTotal;
        game->goldCollected = 0;
        game->spectator = message_noAddr();
        game->mapFileName = mapFileName;

        // CHECK
        FILE* fp = fopen(game->mapFileName, "r");
        game->global = initializeMap(fp);
        fclose(fp);

        if (game->global == NULL){
            fprintf(stderr, "Error: Unable to create grid from map file.");
            exit(1);
        }
        log_v("Generating random number of piles.");
        generateGold(game->global, goldMinNumPiles, goldMaxNumPiles, goldTotal);

        return game;
    }
    // If game can't be properly created
    else{
        log_v("Error: game couldn't be properly created\n");
        return NULL;
    }
}

/****************** new_game ******************/
static int
game_setup(game_t* game)
{
    // Initialize the server
    int portNumber = message_init(NULL);
    if (portNumber == 0) {
        log_v("Unable to initialize port (server.c:game_setup)");
        return 1;
    }

    log_d( "Server port is: %d\n", portNumber);

    // Listen for messages from clients
    log_v("game_setup initializing message loop.");
    bool status = message_loop(game, 0, NULL, NULL, handle_message);

    if (status == false) {
        log_v("Fatal error in message loop (server.c:game_setup:message_loop:handle_message)\n");
    }
    else { log_v("Message loop exited normally.\n"); }

    return 0;
}

/****************** handle_message ******************/
static bool
handle_message(void* arg, addr_t from, const char* message)
{
    // Handling message from client //
    game_t* game = (game_t*)arg;

    // Parse message
    char *messageCopy = calloc(1, strlen(message) + 3);
    strcpy(messageCopy, message);

    char *command, *remainingMessage;
    command = strtok(messageCopy, " ");
    remainingMessage = strtok(NULL, " ");

    // Call handle_message function based on command
    // bool messageAffectsPlayers = true;
    if (strcmp(command, "PLAY") == 0) {
        player_t* player = new_player(remainingMessage, game, from);
        if(player != NULL){
            if(game->playerCount < maxPlayers || player->id == 'Z'){
                char strID[2];     
                strID[0] = player->id;
                strID[1] = '\0';
                
                // Send OK message
                char* okay = malloc(sizeof(char) * (4 + sizeof(strID)));
                strcpy(okay, "OK ");
                strcat(okay, strID);
                message_send(from, okay);

                // Send GRID message
                char *gridMessage = calloc(1, sizeof(char) * 14);
                int NR = grid_getNR(game->global);
                int NC = grid_getNC(game->global);
                sprintf(gridMessage, "GRID %d %d", NR, NC);
                message_send(from, gridMessage);

                // Send GOLD message
                int r = game->goldLeft;
                char *goldMessage = calloc(1, sizeof(char) * 16);
                sprintf(goldMessage, "GOLD 0 0 %d", r);
                message_send(from, goldMessage);

                // Send DISPLAY message
                char* gridString = to_string(game, from);
                message_send(from, gridString);

                free(okay);
                free(gridMessage);
                free(goldMessage);
                free(gridString);
            }
        }else{
            printf("Max players exceeded in handle_message\n");
        }
    }
    else if (strcmp(command, "KEY") == 0) {
        // if key is coming from spectator, send QUIT message
        if(message_eqAddr(game->spectator, from)){

            // if key is either q or Q, quit.
             if (strcmp(remainingMessage, "Q") == 0 || strcmp(remainingMessage, "q") == 0){
                message_send(from, "QUIT Thanks for watching!"); // send spectator QUIT message
                game->spectator = message_noAddr(); // remove spectator
             }
             // otherwise continue spectatting
            else{
                if(!message_eqAddr(game->spectator, message_noAddr())){
                    spectate(game, game->spectator);
                }
            }
        // else, proceed with read_message in handling key movements
        }else{
            log_s("Message: %s\n", remainingMessage);        
            player_t* player = addr_to_player(game, from);
            
            if(player != NULL){
                read_message(game, player, remainingMessage);

                // Send GOLD message
                int n = player->justCollected;
                int p = player->purse;
                int r = game->goldLeft;
                
                char *goldMessage = malloc(sizeof(char) * 16);
                
                sprintf(goldMessage, "GOLD %d %d %d", n, p, r);
                message_send(from, goldMessage);
                
                // Send spectator DISPLAY message
                char* gridString = to_string(game, from);
                message_send(from, gridString);

                if(!message_eqAddr(game->spectator, message_noAddr())){
                    spectate(game, game->spectator);
                }
            
                // Clean up
                free(goldMessage);
                free(gridString);
            }
            
        }
    }else if (strcmp(command, "SPECTATE") == 0) { 
        spectate(game, from);
    }

    free(messageCopy);

    // Break loop if no more gold
    if (game->goldLeft == 0 || (game->quitCount == game->playerCount && game->quitCount > 0)) { return true;  }
    else { return false; }
}

/****************** new_player ******************/
static player_t* 
new_player(char* fullname, game_t* game, addr_t addr){

    // Check if more than 26 players have been created
    if(game->playerCount < 26){

        log_v("Player count less than 26");

        player_t* player = malloc(sizeof(player_t));
        
        // If player could be properly created
        if(player!= NULL){            
            if(message_isAddr(addr)){
                player->port = addr;
            }else{
                player->port = message_noAddr();
            }
            player->name = malloc(sizeof(char) * (strlen(fullname) + 1));
            strcpy(player->name, fullname);
            player->purse = 0;
            player->justCollected = 0;

            // Converting full name to char ID
            char alphabet[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 
                                'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 
                                'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
            int playerCount = game->playerCount;
            log_d("In new_player, player count: %d\n", playerCount);
            char id = alphabet[playerCount];
            player->id = id;        

            // Initially, we give the player the global grid
            player->grid = NULL;
            FILE* fp = fopen(game->mapFileName, "r");
            player->grid = initializeMap(fp);
            fclose(fp);

            // Assign the gold to the player's map
            assign_player_gold(player, game);

            // And assign them a random spot within the global
            if(assign_random_spot(player, game) == true){
                
                // Then we change the visibility to only show their room
                ///player->grid = visualizeMap(player->grid, player->xPos, player->yPos);
                updateMap(player->grid, player->xPos, player->yPos);

                // Update global grid to contain player's assigned random spot
                point_t* globalPoint = grid_get(game->global, player->xPos, player->yPos);
                point_setPlayer(globalPoint, player->id);

                // Adds the player to the array of players in the game
                game->players_array[playerCount] = player;

                // Increment player count
                game->playerCount++;

                return player;

            }else{
                log_v( "Error: Random spot not properly allocated");
                exit(1);
            }
        }else{
            fprintf(stderr, "Error: player (%s) couldn't be properly created\n", fullname);
        }
    }else{
        fprintf(stderr, "Error: maximum number of players exceeded");
        message_send(addr, "QUIT Game is full: no more players can join.");
    }
    return NULL;
}

/****************** assign_player_gold ******************/
static void 
assign_player_gold(player_t* player, game_t* game){
    for(int i = 0; i < grid_getNR(game->global); i++){
        for(int j = 0; j < grid_getNC(game->global); j++){
            point_t* globalPoint = grid_get(game->global, i, j);
            int amount = point_getGold(globalPoint);
            if(amount > 0){
                point_t* localPoint = grid_get(player->grid, i, j);
                point_setGold(localPoint, amount);
                grid_insert(player->grid, localPoint, i, j);
            }
        }
    }
}

/****************** assign_random_spot ******************/
static bool 
assign_random_spot(player_t* player, game_t* game){
    srand(time(NULL));

    log_v("Beginning of assign random spot\n");

    bool assignedSpot = false;

    int NR = grid_getNR(game->global);              // Number of rows in global grid
    int NC = grid_getNC(game->global);              // Number of columns in global grid

    // While coordinates haven't been assigned to this player
    while(assignedSpot == false){

        // generate a random x-coordinate between 0 and NR
        int xRand = rand() % NR;
        // generate a random y-coordinate between 0 and NC
        int yRand = rand() % NC;
        
        // Get the value of that point in the map
        point_t* randPoint = grid_get(game->global, xRand, yRand);
        int pointVal = point_getVal(randPoint);

        // If the spot is valid
        if(pointVal == 1){

            // Give the player those coordinates
            player->xPos = xRand;
            player->yPos = yRand;

            // Update GLOBAL player ID
            randPoint->id = player->id;

            assignedSpot = true;
        }
    }

    log_v("Inside assign random spot\n");
    
    // If the spot was NOT properly assigned
    if(player->xPos == 0 && player->yPos == 0){
        return false;
    }
    // Else if the spot was properly assigned
    else{
        return true;
    }
}

/****************** to_string ******************/
static char* to_string(game_t* game, addr_t from){
    grid_t* localGrid = NULL;
    grid_t* globalGrid = game->global;
    player_t* player = addr_to_player(game, from);

    bool spectator = false;

    // If the address matches a player in the game
    if(player != NULL){
        localGrid = player->grid;
    }else{
        spectator = true;
    }

    int NR = grid_getNR(globalGrid);
    int NC = grid_getNC(globalGrid);
    int total = ((NR+1)*NC)+9;

    char* output = malloc(sizeof(char) * (total + 1));
    
    if(output != NULL){
        fflush(stdout);

        strcpy(output, "DISPLAY\n");

        //Concatenate the grid to the output
        for (int i = 0; i < NR; i++){
            for (int j = 0; j < NC; j++){
                
                if(spectator == false){
                    point_t* localPoint = grid_get(localGrid, i, j);
                    point_t* globalPoint = grid_get(globalGrid, i, j);

                    // If the point is visible
                    if(point_getTrack(localPoint) == true){

                        // If a player DOES NOT exist at that spot
                        if(point_getPlayer(globalPoint) == ' '){

                            // If gold exists at that spot
                            if(localPoint->invisibleGold == true){
                                int numVal = point_getVal(localPoint);
                            
                                if(numVal == 0){
                                    strcat(output, " ");
                                }
                                else if(numVal == 1){
                                    strcat(output, ".");
                                }
                                else if(numVal == 2){
                                    strcat(output, "-");
                                }
                                else if(numVal == 5){
                                    strcat(output, "|");
                                }
                                else if(numVal == 3){
                                    strcat(output, "#");
                                }
                                else if(numVal == 4){
                                    strcat(output, "+");
                                }
                                //
                            }else if(point_getGold(globalPoint) > 0){
                                strcat(output, "*");
                            }
                            else{

                                int numVal = point_getVal(localPoint);
                            
                                if(numVal == 0){
                                    strcat(output, " ");
                                }
                                else if(numVal == 1){
                                    strcat(output, ".");
                                }
                                else if(numVal == 2){
                                    strcat(output, "-");
                                }
                                else if(numVal == 5){
                                    strcat(output, "|");
                                }
                                else if(numVal == 3){
                                    strcat(output, "#");
                                }
                                else if(numVal == 4){
                                    strcat(output, "+");
                                }
                            }
                            
                        }else{
                            
                            char spotID = point_getPlayer(globalPoint);
                            
                            // If we're not in specatator mode
                            if(spectator == false){
                                char currId = player->id;

                                // If the player at the spot is the current player
                                if(currId == spotID){
                                    // We want to show the "@" symbol
                                    strcat(output, "@");
                                }else{
                                    strcat(output, &spotID);
                                }
                            }
                            // We will display the other player's ID
                            else{
                                strcat(output, &spotID);
                            }
                        }
                    }else{
                        strcat(output, " ");
                    }
                }
                // Otherwise if spectate is true
                else{
                    point_t* globalPoint = grid_get(globalGrid, i, j);

                    // If no player exists at that point
                    if(point_getPlayer(globalPoint) == ' '){

                        // If gold exists at that spot
                        if(point_getGold(globalPoint) > 0){
                            strcat(output, "*");
                        }
                        // Else set value to current value
                        else{

                            int numVal = point_getVal(globalPoint);
                        
                            if(numVal == 0){
                                strcat(output, " ");
                            }
                            else if(numVal == 1){
                                strcat(output, ".");
                            }
                            else if(numVal == 2){
                                strcat(output, "-");
                            }
                            else if(numVal == 5){
                                strcat(output, "|");
                            }
                            else if(numVal == 3){
                                strcat(output, "#");
                            }
                            else if(numVal == 4){
                                strcat(output, "+");
                            }
                        }
                        
                    }else if(point_getPlayer(globalPoint) != ' '){       
                        char spotID = point_getPlayer(globalPoint);
                        char strID[2];     
                        strID[0] = spotID;
                        strID[1] = '\0';
                        
                        strcat(output, strID);
                    }
                }
            }
            strcat(output, "\n");
        }
        return output;
    }
    fprintf(stderr, "Error: couldn't allocate memory for display string\n");
    return NULL;
}

/****************** id_to_player ******************/
static player_t*
id_to_player(game_t* game, char id){
    player_t** players = game->players_array;
    for(int i = 0; i < game->playerCount; i++){
        if(players[i]->id == id){
            return players[i];
        }
    }
    return NULL;
}

/****************** addr_to_player ******************/
static player_t*
addr_to_player(game_t* game, addr_t addr){    
    for(int i = 0; i < game->playerCount; i++){
        if(message_eqAddr(addr, game->players_array[i]->port) == true){
            return game->players_array[i];
        }
    }
    return NULL;
}

/****************** read_message ******************/
static void 
read_message(game_t* game, player_t* player, char* message){
    
    if(player != NULL){
        int currX = player->xPos;
        int currY = player->yPos;

        int newX;
        int newY;

        log_v("Reading message: Set old and move coords to NULL\n");

        if (strcmp(message, "Q") == 0 || strcmp(message, "q") == 0) {            
            log_v("Received key q\n");
            game->quitCount++;
            if(game->playerCount == game->quitCount){
                fflush(stdout);
            }else{
                // Otherwise for players
                message_send(player->port, "QUIT Thanks for playing!");
            }
        }

        // k move down, if possible
        if(strcmp(message, "k") == 0){
            newX = currX - 1;
            newY = currY;
            move_player(game, player, currX, currY, newX, newY);
        }
        // j move up, if possible
        else if(strcmp(message, "j") == 0){
            newX = currX + 1;
            newY = currY;
            move_player(game, player, currX, currY, newX, newY);
        }
        // l move left, if possible
        else if(strcmp(message, "l") == 0){
            newX = currX;
            newY = currY + 1;
            move_player(game, player, currX, currY, newX, newY);
        }  
        // h move right , if possible
        else if(strcmp(message, "h") == 0){
            newX = currX;
            newY = currY - 1;
            move_player(game, player, currX, currY, newX, newY);
        }
        // y move diagonally up and left, if possible
        else if(strcmp(message, "y") == 0){
            newX = currX - 1;
            newY = currY - 1;
            move_player(game, player, currX, currY, newX, newY);
        }
        // b move diagonally down and left, if possible
        else if(strcmp(message, "b") == 0){
            newX = currX + 1;
            newY = currY - 1;
            move_player(game, player, currX, currY, newX, newY);
        }
        // u move diagonally right and right, if possible
        else if(strcmp(message, "u") == 0){
            newX = currX - 1;
            newY = currY + 1;
            move_player(game, player, currX, currY, newX, newY);
        }
        // n move diagonally down and right, if possible
        else if(strcmp(message, "n") == 0){
            newX = currX + 1;
            newY = currY + 1;
            move_player(game, player, currX, currY, newX, newY);
        }


        // K move max down, if possible
        if(strcmp(message, "K") == 0){
            newX = currX - 1;
            newY = currY;
            bool possible = move_player(game, player, currX, currY, newX, newY);
            while (possible){
                currX = newX;
                currY = newY;
                newX--;
                possible = move_player(game, player, currX, currY, newX, newY);                
            }
        }
        // J move max up, if possible
        else if(strcmp(message, "J") == 0){
            newX = currX + 1;
            newY = currY;
            bool possible = move_player(game, player, currX, currY, newX, newY);
            while (possible){
                currX = newX;
                currY = newY;
                newX++;
                possible = move_player(game, player, currX, currY, newX, newY);                
            }
        }
        // L move max left, if possible
        else if(strcmp(message, "L") == 0){
            newX = currX;
            newY = currY + 1;
            bool possible = move_player(game, player, currX, currY, newX, newY);
            while (possible){
                currX = newX;
                currY = newY;
                newY++;    
                possible = move_player(game, player, currX, currY, newX, newY);                
            }
        }  
        // H move max right , if possible
        else if(strcmp(message, "H") == 0){
            newX = currX;
            newY = currY - 1;
            bool possible = move_player(game, player, currX, currY, newX, newY);
            while (possible){
                currX = newX;
                currY = newY;
                newY--;    
                possible = move_player(game, player, currX, currY, newX, newY);                
            }
        }
        // Y move max diagonally up and left, if possible
        else if(strcmp(message, "Y") == 0){
            newX = currX - 1;
            newY = currY - 1;
            bool possible = move_player(game, player, currX, currY, newX, newY);
            while (possible){
                currX = newX;
                currY = newY;
                newX--;
                newY--;    
                possible = move_player(game, player, currX, currY, newX, newY);                
            }
        }
        // B move max diagonally down and left, if possible
        else if(strcmp(message, "B") == 0){
            newX = currX + 1;
            newY = currY - 1;
            bool possible = move_player(game, player, currX, currY, newX, newY);
            while (possible){
                currX = newX;
                currY = newY;
                newX++;
                newY--;    
                possible = move_player(game, player, currX, currY, newX, newY);                
            }
        }


        // N move max diagonally down and right, if possible
        else if(strcmp(message, "N") == 0){
            newX = currX + 1;
            newY = currY + 1;
            bool possible = move_player(game, player, currX, currY, newX, newY);
            while (possible){
                currX = newX;
                currY = newY;
                newX++;
                newY++;    
                possible = move_player(game, player, currX, currY, newX, newY);                
            }
        }
        // U move max diagonally up and right, if possible
        else if(strcmp(message, "U") == 0){
            newX = currX - 1;
            newY = currY + 1;
            bool possible = move_player(game, player, currX, currY, newX, newY);
            while (possible){
                if(possible == true){printf("POSSIBLE\n");}
                else{printf("not possible\n");}
                currX = newX;
                currY = newY;
                newX--;
                newY++;    
                possible = move_player(game, player, currX, currY, newX, newY);                
            }
        }
    }
    return;
}

/****************** move_player ******************/
static bool 
move_player(game_t* game, player_t* player, int currX, int currY, int newX, int newY){
    
    grid_t* localGrid = player->grid;
    grid_t* globalGrid = game->global;

    point_t* oldPointLocal = grid_get(localGrid, currX, currY);
    point_t* newPointLocal = grid_get(localGrid, newX, newY);

    point_t* oldPointGlobal = grid_get(globalGrid, currX, currY);
    point_t* newPointGlobal = grid_get(globalGrid, newX, newY);

    // If a player is there
    if(point_getPlayer(newPointGlobal) != ' '){
        // SWAP the two players
        char otherId = point_getPlayer(newPointGlobal);
        player_t* otherPlayer = id_to_player(game, otherId);
        
        // GLOBALLY Set the old player to the old coordinates
        otherPlayer->xPos = currX;
        otherPlayer->yPos = currY;
        point_setPlayer(oldPointGlobal, otherId);

        // GLOBALLY Set the current player to the new coordinates 
        player->xPos = newX;
        player->yPos = newY;
        point_setPlayer(newPointGlobal, player->id);

        // LOCALLY Set the current player to the new coordinates
        point_setPlayer(newPointLocal, player->id);

        // LOCALLY Remove the current player from the old coordinates
        point_setPlayer(oldPointLocal, ' ');
        
        updateMap(player->grid, player->xPos, player->yPos);
        return true;
    }
    // If another player isn't at that spot
    else{
        // If it is a room or a passage
        if(point_getVal(newPointLocal) == 1 || point_getVal(newPointLocal) == 3){
            // Move the player there
            player->xPos = newX;
            player->yPos = newY;
            point_setPlayer(newPointLocal, player->id);
            point_setPlayer(newPointGlobal, player->id);

            // Collect the gold
            if(point_getGold(newPointGlobal) > 0){
                // Give the player the gold
                player->purse = player->purse + point_getGold(newPointGlobal);
                player->justCollected = point_getGold(newPointGlobal);
                
                game->goldCollected = game->goldCollected + player->justCollected;
                game->goldLeft = goldTotal - game->goldCollected;

                goldCollected = goldCollected + player->justCollected;

                // Update the point to have no gold left
                point_setGold(newPointGlobal, 0);
            }

            // Remove the player from the old spot
            point_setPlayer(oldPointLocal, ' ');
            point_setPlayer(oldPointGlobal, ' ');
            updateMap(player->grid, player->xPos, player->yPos);
            return true;
        }
        else{
            updateMap(player->grid, player->xPos, player->yPos);
            return false;
            }

    }
    
}

/****************** spectate ******************/
static void
spectate( game_t* game, addr_t address){    
    // If there isn't a spectator before;
    addr_t oldAddr = game->spectator;
    // const char* addrStr = message_stringAddr(oldAddr);
    if (!message_eqAddr(oldAddr, address)){
         // Kick them off
        message_send(oldAddr, "QUIT You have been replaced by a new spectator.");
        // Replace the game's old spectator with the new one
        game->spectator = address;
        oldAddr = address;
    }else if(message_eqAddr(address, message_noAddr())){
        game->spectator = address;
    // If the old spectator is different than the new one
    } 
    // Otherwise display the same for the spectator
    
    // Send GRID message
    char *gridMessage = calloc(1, sizeof(char) * 14);
    int NR = grid_getNR(game->global);
    int NC = grid_getNC(game->global);
    sprintf(gridMessage, "GRID %d %d", NR, NC);
    message_send(address, gridMessage);

    // Send GOLD message
    int r = game->goldLeft;
    char *goldMessage = calloc(1, sizeof(char) * 16);
    sprintf(goldMessage, "GOLD 0 0 %d", r);
    message_send(address, goldMessage);

    // Send DISPLAY message
    char* gridString = to_string(game, address);
    message_send(address, gridString);
    
    // Clean up
    free(gridMessage);
    free(goldMessage);
    free(gridString);
}

/****************** validate_seed ******************/
static int
validate_seed(char* optionalSeed)
{
    int i = 0;
    char c;

    // Confirm all characters are digits
    while ((c = optionalSeed[i]) != '\0') {
        if (isdigit(c) == 0) { return -1; }
        i ++;
    }

    int seed = atoi(optionalSeed); // convert to integer
    log_d("Seed as integer: %d", seed);

    // Confirm seed is a positive integer
    if (seed < 0) { return -1; }
    else { return seed; }
}

/****************** leave_game ******************/
static void 
leave_game(game_t* game){
    // Compiling the end of game table //
    fflush(stdout);

    char game_summary[1000];
    memset(game_summary, 0, sizeof(game_summary));

    char* header = "QUIT GAME OVER:\n";
    strcat(game_summary, header);

    fflush(stdout);

    for (int i = 0; i < game->playerCount; i++) {
        
        player_t * player = game->players_array[i];
        if(player != NULL){

            log_s("Player name (for end of game table): %s\n", player->name);

            char line[100];
            sprintf(line, "%c\t%d\t%s\n", player->id, player->purse, player->name);

            strcat(game_summary, line);

            //free(key);
        }
    }

    player_t** players_array = game->players_array;


    char* concatenate_game_summary = calloc(1, strlen(game_summary) + 1);
    strcpy(concatenate_game_summary, game_summary);

    // Sending end of game table //
    for (int i = 0; i < game->playerCount; i++) {
        if(message_stringAddr(players_array[i]->port) != NULL){
            message_send(players_array[i]->port, concatenate_game_summary);
        }
    }
    
    addr_t spectator = game->spectator;
    //if (message_isAddr(spectator) && !message_eqAddr(spectator, message_noAddr())) {
    if (message_isAddr(spectator)) {
        message_send(spectator, concatenate_game_summary);
    }

    log_v("Freeing memory for end of game table (in endOfGame)");
    free(concatenate_game_summary);
}

/****************** remove_player ******************/
static void 
remove_player(player_t* player, game_t* game){

    // Delete the grid
    delete_grid(player->grid);
    
    // Delete the player from the game
    for(int i = 0; i < game->playerCount; i++){
        if(game->players_array[i] != NULL){
            if(game->players_array[i]->id == player->id){
                game->players_array[i] = NULL;
            }
        }
    }

    // Free the name
    free(player->name);

    fflush(stdout);
    free(player);

    game->playerCount--;
}

/****************** delete_game ******************/
static void 
delete_game(game_t* game){

    // Delete each player in the array
    for(int i = 0; i < 26; i++){
        if(game->players_array[i] != NULL){
            remove_player(game->players_array[i], game);
        }
    }

    // Delete the grid
    delete_grid(game->global);

    // Delete the array of players
    free(game->players_array);

    free(game);
}