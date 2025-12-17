#include "board.h"
#include <stdlib.h>

int main(void) {
  Board *board = (Board *)malloc(sizeof(Board));
  init_board(board);
  print_board(board);

  return 0;
}

