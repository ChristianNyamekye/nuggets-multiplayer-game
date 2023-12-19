#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

int n;
int r;
int p;

static void readGold(char* goldMessage);

int main()
{
  char* msg1 = "GOLD 7 19 148";
  char* msg2 = "GOLD 18 10 182";
  char* msg3 = "GOLD 61 4 185";
  char* msg4 = "GOLD 9 231 10";

  readGold(msg1);
  readGold(msg2);
  readGold(msg3);
  readGold(msg4);

  return 0;
}



static void
readGold(char* goldMessage)
{
  int i = 0;
  char* nug = malloc(sizeof(char*));
  char* purse = malloc(sizeof(char*));
  char* rem = malloc(sizeof(char*));
  char* temp1 = malloc(sizeof(goldMessage));

  int endLen;

  endLen = strlen(goldMessage) - strlen("GOLD ");

  strcpy(temp1, goldMessage + strlen(goldMessage) - endLen);

  while (!isspace(temp1[i])) {
    i++;
  }

  for (int t = 0; t < 10; t++) {
    purse[t] = ' ';
    rem[t] = ' ';

    if (t < i + 2) {
      nug[t] = ' ';
    }
  }

  strncpy(nug, temp1, i);
  nug[i+1] = '\0';

  int j = i + 1;

  while (!isspace(temp1[j])) {
    j++;
  }

  strncpy(purse, temp1 + 1 + i, j - i - 1);
  purse[j-i-1] = '\0';

  int k = j + 1;

  while (k < strlen(temp1) && !isspace(temp1[k])) {
    k++;
  }

  strncpy(rem, temp1 + j + 1, k - j);
  rem[k - j + 1] = '\0';

  //Add spectator stuff

  n = atoi(nug);
  p = atoi(purse);
  r = atoi(rem);

  /*
  if (n > 0) {
    mvprintw(0, ncols/2, "Player %c has %d nuggets (%d unclaimed). GOLD received: %d", player_char, p, r, n);
  }
  else {
  mvprintw(0, ncols/2, "Player %c has %d nuggets (%d unclaimed).", player_char, p, r);
  }
  */

  printf("new gold: %d \npurse: %d \nremaining: %d \n", n, p, r);

  free(nug);
  free(purse);
  free(rem);
  free(temp1);
}