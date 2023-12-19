#!/bin/bash
chmod +x testing.sh
#unit testing
#
#script name: testing.sh
#
#summary: This executes a sequence of commands that demonstrate that server works with edge cases.
#
#Christian Nyamekye, Spring 2023



# Test with no arguments
./server 

if [ $? -eq 1 ]
then
    echo passed test with no arguments
else
    echo failed test with no arguments
fi

####### Arguments Test ######

# Test with more than 2 arguments
./server ../maps/main.txt 1111  hat

if [ $? -eq 1 ]
then
    echo passed test with more than 2 arguments
else
    echo failed test with more than 2 arguments
fi



#test querier with invalid mapfile

./server ../maps/wrong.txt 
if [ $? -eq 2 ]
then
    echo Passed test with invalid mapfile
else
    echo did not pass test with invalid mapfile
fi

#test with Invalid seed
./server ../maps/main.txt 12c4
if [ $? -eq 1 ]
then
    echo Passed test with invalid optional seed
else
    echo did not pass test with invalid optional seed
fi