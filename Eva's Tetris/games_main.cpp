#include <stdlib.h>
#include <stdio.h>
#include "tetris.h"

int main(int argc, char** argv)
{
  // A small program to test a player's tetris skills.
  puts("Welcome to Terminal Tetris!");
  puts("Space to rotate peice, arrows to move. Good luck!");
  puts("=== Press Enter to Continue ===");
  getchar();
  int score = Tetris();
  printf("Final Score :: %d\nThanks for playing!\n", score);
  return 0;
}
