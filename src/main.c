#include "board.h"
#include "move.h"

int main(void) {
  Board *board = (Board *)malloc(sizeof(Board));
  init_board(board);
  print_board(board);

  MoveList *initial_removals = generate_initial_removals(board, true);
  print_move_list(initial_removals);

  return 0;
}

