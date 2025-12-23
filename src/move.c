#include "move.h"

/* Direction vectors: up, right, down, left */
static const int dir_row[4] = {-1, 0, 1, 0};
static const int dir_col[4] = {0, 1, 0, -1};

Move create_simple_jump(int from_row, int from_col,
                        int to_row, int to_col,
                        int captured_row, int captured_col,
                        int direction) {
  int from_idx = coord_to_index(from_row, from_col);
  int to_idx = coord_to_index(to_row, to_col);
  int captured_idx = coord_to_index(captured_row, captured_col);

  return MOVE_ENCODE(from_idx, to_idx, captured_idx, 1, direction);
}

Move create_initial_removal(int row, int col) {
  int idx = coord_to_index(row, col);
  return MOVE_INITIAL_REMOVAL_ENCODE(idx);
}

void dfs_jumps(const Board *board,
               int row, int col,
               bool is_white_turn,
               MoveSequence *current,
               MoveSequence *results,
               int *result_count) {
  bool found_jump = false;

  for (int dir = 0; dir < 4; dir++) {
    int over_row = row + dir_row[dir];
    int over_col = col + dir_col[dir];
    int land_row = row + 2 * dir_row[dir];
    int land_col = col + 2 * dir_col[dir];

    if (!is_valid_position(over_row, over_col) ||
        !is_valid_position(land_row, land_col))
      continue;

    bool has_opponent = is_white_turn
      ? is_black(board, over_row, over_col)
      : is_white(board, over_row, over_col);

    if (!has_opponent || !is_empty(board, land_row, land_col))
      continue;

    found_jump = true;

    Board next = *board;

    remove_stone(&next, row, col);
    remove_stone(&next, over_row, over_col);

    if (is_white_turn)
      set_white(&next, land_row, land_col);
    else
      set_black(&next, land_row, land_col);

    Move jump = create_simple_jump(
      row, col,
      land_row, land_col,
      over_row, over_col,
      dir
    );

    if (current->count >= MAX_MOVES)
      continue;

    current->jumps[current->count++] = jump;

    dfs_jumps(&next,
              land_row, land_col,
              is_white_turn,
              current,
              results,
              result_count);

    current->count--;
  }

  if (!found_jump && current->count > 0) {
    results[*result_count] = *current;
    (*result_count)++;
  }
}

int generate_all_moves(const Board *board,
                       bool is_white_turn,
                       MoveSequence *out_moves) {
  int count = 0;
  Bitboard pieces = is_white_turn ? board->white : board->black;

  while (pieces) {
    int idx = pop_lsb(&pieces);
    int row, col;
    index_to_coord(idx, &row, &col);

    MoveSequence current;
    current.count = 0;

    dfs_jumps(board,
              row, col,
              is_white_turn,
              &current,
              out_moves,
              &count);
  }

  return count;
}

bool execute_sequence(Board *board,
                      const MoveSequence *seq,
                      bool is_white_turn) {
  for (int i = 0; i < seq->count; i++) {
    Move m = seq->jumps[i];

    int fr, fc, tr, tc, cr, cc;
    index_to_coord(MOVE_FROM(m), &fr, &fc);
    index_to_coord(MOVE_TO(m), &tr, &tc);
    index_to_coord(MOVE_CAPTURED(m), &cr, &cc);

    remove_stone(board, fr, fc);
    remove_stone(board, cr, cc);

    if (is_white_turn)
      set_white(board, tr, tc);
    else
      set_black(board, tr, tc);
  }
  return true;
}

bool execute_initial_removal(Board *board,
                             int row, int col,
                             bool is_black) {
  if (!is_valid_initial_removal(board, row, col, is_black))
    return false;

  Bitboard mask = get_bitmask(row, col);

  if (is_black)
    board->black &= ~mask;
  else
    board->white &= ~mask;

  board->occupied &= ~mask;
  board->empty |= mask;

  return true;
}

bool is_valid_initial_removal(const Board *board,
                              int row, int col,
                              bool is_black) {
  if (!is_valid_position(row, col))
    return false;

  Bitboard mask = get_bitmask(row, col);

  if (is_black) {
    int c = BOARD_SIZE / 2;

    bool corner =
      (row == 0 && col == 0) ||
      (row == 0 && col == BOARD_SIZE - 1) ||
      (row == BOARD_SIZE - 1 && col == 0) ||
      (row == BOARD_SIZE - 1 && col == BOARD_SIZE - 1);

    bool center = (row == c && col == c);

    if (!(corner || center))
      return false;

    return (board->black & mask);
  }

  if (popcount(board->empty) != 1)
    return false;

  int idx = __builtin_ctzll(board->empty);
  int br, bc;
  index_to_coord(idx, &br, &bc);

  if (!((row == br && abs(col - bc) == 1) ||
        (col == bc && abs(row - br) == 1)))
    return false;

  return (board->white & mask);
}
