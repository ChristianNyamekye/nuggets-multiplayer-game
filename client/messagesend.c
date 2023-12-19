#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main()
{
  char* one = "KEY ";
  char c = 'j';
  char* temp = malloc(strlen(one) + 2);

  strcpy(temp, one);
  temp[strlen(one)] = c;
  temp[strlen(one) + 1] = '\0';

  fprintf(stdout, "%s \n", temp);
  free(temp);

  return 0;
}