#include "game.h"

void run_game(void) {
  Board *board = (Board *)malloc(sizeof(Board));
  init_board(board);
  print_board(board);

  int row, col;
  do {
    printf("Black: remove a stone (corner/center): ");
  } while (!read_coord(&row, &col) || !execute_initial_removal(board, row, col, true));

  print_board(board);

  do {
    printf("White: remove a stone adjacent to black's removal: ");
  } while (!read_coord(&row, &col) || !execute_initial_removal(board, row, col, false));

  print_board(board);

  bool is_white_turn = false;
  while (true) {
    print_board(board);

    MoveSequence seq;
    if (!human_choose_move(board, is_white_turn, &seq))
      break;

    execute_sequence(board, &seq, is_white_turn);
    is_white_turn = !is_white_turn;
  }

  free(board);
  printf("Game over!\n");
}


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

#include <stdio.h>

bool human_choose_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq) {
  char from_col_char, to_col_char;
  int from_row, to_row;

  MoveSequence all_moves[MAX_MOVES];
  int num_moves = generate_all_moves(board, is_white_turn, all_moves);

  if (num_moves == 0)
    return false;

  while (true) {
    printf("%s move: ", is_white_turn ? "White" : "Black");
    if (scanf(" %c%d %c%d", &from_col_char, &from_row, &to_col_char, &to_row) != 4) {
      printf("Invalid input format!\n");
      while (getchar() != '\n');
      continue;
    }

    from_col_char = toupper(from_col_char);
    to_col_char   = toupper(to_col_char);
    int from_col = from_col_char - 'A';
    int to_col   = to_col_char - 'A';
    int from_r = from_row - 1;
    int to_r   = to_row - 1;

    if (!is_valid_position(from_r, from_col) || !is_valid_position(to_r, to_col)) {
      printf("Coordinates out of bounds!\n");
      continue;
    }

    int from_idx = coord_to_index(from_r, from_col);
    int to_idx   = coord_to_index(to_r, to_col);

    for (int i = 0; i < num_moves; i++) {
      if (all_moves[i].count == 0)
        continue;
      if (MOVE_FROM(all_moves[i].jumps[0]) == (unsigned int)from_idx &&
          MOVE_TO(all_moves[i].jumps[0]) == (unsigned int)to_idx) {
        *chosen_seq = all_moves[i];
        return true;
      }
    }

    printf("No valid move from %c%d to %c%d\n", from_col_char, from_row, to_col_char, to_row);
  }
}
