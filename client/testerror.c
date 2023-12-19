#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main()
{
  char* one = "ERROR invalid keystroke w";
  char* two = malloc(sizeof(char*));

  int endLen = strlen(one) - strlen("ERROR ");
  int totalLen = strlen(one);

  printf("%d \n", endLen);
  printf("%d \n", totalLen);

  strcpy(two, one + totalLen - endLen);
  printf("%s \n", two);

  free(two);

  return 0;
}