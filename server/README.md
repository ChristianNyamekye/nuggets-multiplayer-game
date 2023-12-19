# README for Server
## Bash-Brigade: Nuggets Final Project CS50 23S
### Primary Author: Christian Nyamekye

*server* is responsible for running the nuggest game and handling messages from clients. Greater detail on the protocol for these messages and the internal structure of *server* can be found in REQUIREMENTS.md, DESIGN.md, and IMPLEMENTATION.md.

*server* is dependent on the following modules and data structures: *support*, *grid*, *player*, *player*, and *game*.

To compile run: `make`
To run the program with the main map: `make mainmap`
To execute the testing script: `make test`. See output in a `testing.out` file you must create

The following commands run variations of `valgrind` on *server* with `../maps/small.txt`. Output is saved to `log.txt`.
1. `make vlagrind`
2. `make valgrindFull`
3. `make valgrindAll`

To clean: `make clean` 