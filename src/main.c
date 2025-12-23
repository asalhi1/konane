#include "board.h"
#include "move.h"

// placeholder here until implement UI

#include <ctype.h>

bool read_coord(int *row, int *col) {
  char col_char;
  int row_num;

  if (scanf(" %c%d", &col_char, &row_num) != 2)
    return false;

  col_char = toupper(col_char);

  if (col_char < 'A' || col_char >= 'A' + BOARD_SIZE)
    return false;

  if (row_num < 1 || row_num > BOARD_SIZE)
    return false;

  *col = col_char - 'A';
  *row = row_num - 1;
  return true;
}

int main(void) {
  Board *board = malloc(sizeof(Board));
  if (!board)
    return 1;

  init_board(board);
  print_board(board);

  int row, col;

  do {
    printf("Black remove a stone from the corners or center: ");
  } while (!read_coord(&row, &col) ||
           !execute_initial_removal(board, row, col, true));

  do {
    printf("White: remove a stone adjacent to Black: ");
  } while (!read_coord(&row, &col) ||
           !execute_initial_removal(board, row, col, false));

  print_board(board);

  free(board);
  return 0;
}
