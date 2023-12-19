#!/bin/bash
#Author: Caleb Ash
#Date: May, 2023
#Testing script for nuggets game - client

#Testing with incorrect command line entries

#Testing too few command line entries
./client plank

#Testing with too many command line entries
./client plank 71826 Caleb Hello Bad

#Testing with a port number that's bad
./client plank badPort Caleb