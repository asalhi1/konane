#include "move.h"

// Direction vectors: up, right, down, left
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

MoveList *create_move_list() {
  MoveList *list = (MoveList*)malloc(sizeof(MoveList));
  if (!list) return NULL;

  list->head = list->tail = NULL;
  list->count = 0;
  return list;
}

void free_move_list(MoveList *list) {
  if (!list) return;

  MoveNode *current = list->head;
  while (current) {
    MoveNode* next = current->next;
    free(current);
    current = next;
  }

  free(list);
}

void add_move(MoveList *list, Move move) {
  MoveNode *node = (MoveNode*)malloc(sizeof(MoveNode));
  if (!node) return;

  node->move = move;
  node->next = NULL;

  if (list->tail) {
    list->tail->next = node;
    list->tail = node;
  } else {
    list->head = list->tail = node;
  }

  list->count++;
}

void add_move_coords(MoveList *list, int from_row, int from_col,
                     int to_row, int to_col,
                     int captured_row, int captured_col,
                     int direction) {
  Move move = create_simple_jump(from_row, from_col,
                                 to_row, to_col,
                                 captured_row, captured_col,
                                 direction);
  add_move(list, move);
}

Move pop_move(MoveList *list) {
  if (!list || !list->head) return 0;

  MoveNode *node = list->head;
  Move move = node->move;
  list->head = node->next;

  if (!list->head) list->tail = NULL;

  free(node);
  list->count --;
  return move;
}

MoveList *generate_moves_for_stone(const Board *board,
                                   int from_row, int from_col, 
                                   bool is_white_turn) {
  MoveList *moves = create_move_list();
  if (!moves) return NULL;

  for (int dir = 0; dir < 4; dir ++) {
    int jump_over_row = from_row + dir_row[dir];
    int jump_over_col = from_col + dir_col[dir];
    int land_row = from_row + 2 * dir_row[dir];
    int land_col = from_col + 2 * dir_col[dir];
    
    if (!is_valid_position(jump_over_row, jump_over_col) ||
        !is_valid_position(land_row, land_col)) continue;
  
    bool has_opponent = is_white_turn ?
      is_black(board, jump_over_row, jump_over_col) :
      is_white(board, jump_over_row, jump_over_col);

    bool landing_empty = is_empty(board, land_row, land_col);

    if (has_opponent && landing_empty) {
      add_move_coords(moves, from_row, from_col,
                      land_row, land_col,
                      jump_over_row, jump_over_col,
                      dir);
    }
  }

  return moves;
}

MoveList *generate_initial_removals(const Board *board, bool is_black) {
  MoveList *moves = create_move_list();
  if (!moves) return NULL;

  if (is_black) {
    int positions[5][2] = {
      {0, 0},
      {0, BOARD_SIZE - 1},
      {BOARD_SIZE - 1, 0},
      {BOARD_SIZE - 1, BOARD_SIZE - 1},
      {BOARD_SIZE / 2, BOARD_SIZE / 2}
    };
      
    for (int i = 0; i < 5; i++) {
      int row = positions[i][0];
      int col = positions[i][1];

      Bitboard mask = get_bitmask(row, col);
      if (mask && (board->black & mask)) {
        add_move(moves, create_initial_removal(row, col));
      }
    }
  } else {
    // find black's removed piece 
    Bitboard piece = ~board->occupied & VALID_MASK;
    //if (piece && !(piece & (piece - 1))) {
    int idx = __builtin_ctz(piece);
    //}

    int black_row, black_col;
    index_to_coord(idx, &black_row, &black_col);

    // add adjacent pieces
    for (int dir = 0; dir < 4; dir ++) {
      int adj_row = black_row + dir_row[dir];
      int adj_col = black_col + dir_col[dir];
      
      if (is_valid_position(adj_row, adj_col) && is_white(board, adj_row, adj_col)) {
        Move move = create_initial_removal(adj_row, adj_col);
        add_move(moves, move);
      }
    }
  }

  return moves;
}

MoveList *generate_all_moves(const Board *board, bool is_white_turn) {
  MoveList *all_moves = create_move_list();
  if (!all_moves) return NULL;

  Bitboard player_stones = is_white_turn ? board->white : board->black;
  Bitboard temp = player_stones;

  while (temp) {
    int idx = pop_lsb(&temp);
    int row, col;
    index_to_coord(idx, &row, &col);

    MoveList *stone_moves = generate_moves_for_stone(board, row, col, is_white_turn);

    if (stone_moves) {
      while (stone_moves->head) {
        Move move = pop_move(stone_moves);
        add_move(all_moves, move);
      }
      free_move_list(stone_moves);
    }
  }

  return all_moves;
}

bool execute_move(Board *board, Move move, bool is_white_turn) {
  if (IS_INITIAL_REMOVAL(move)) {
    int idx = INITIAL_REMOVAL_POS(move);
    int row, col;
    index_to_coord(idx, &row, &col);
    return execute_initial_removal(board, row, col, is_white_turn);
  }

  int from_idx = MOVE_FROM(move);
  int to_idx = MOVE_TO(move);
  int captured_idx = MOVE_CAPTURED(move);

  int from_row, from_col, to_row, to_col, captured_row, captured_col;
  index_to_coord(from_idx, &from_row, &from_col);
  index_to_coord(to_idx, &to_row, &to_col);
  index_to_coord(captured_idx, &captured_row, &captured_col);

  remove_stone(board, from_row, from_col);
  
  if (is_white_turn) {
    set_white(board, to_row, to_col);
  } else {
    set_black(board, to_row, to_col);
  }

  remove_stone(board, captured_row, captured_col);

  return true;
}

bool execute_initial_removal(Board *board, int row, int col, bool is_black) {
  if (!board) {
    printf("Error: Invalid board pointer\n");
    return false;
  }
  
  if (!is_valid_position(row, col)) {
    printf("Error: Position (%d, %d) is outside the board\n", row, col);
    return false;
  }
  
  Bitboard mask = get_bitmask(row, col);
  if (!mask) {
    printf("Error: Invalid bitmask for position (%d, %d)\n", row, col);
    return false;
  }
  
  if (is_black) {
      if (!is_valid_initial_removal(board, row, col, true)) {
        printf("Error: Black can only remove from corners (A1, A7, G1, G7) or center (D4)\n");
        printf("You tried: %c%d\n", 'A' + col, row + 1);
        return false;
      }
      
      if (!(board->black & mask)) {
        printf("Error: No black stone at %c%d\n", 'A' + col, row + 1);
        return false;
      }
      
      if (board->empty & mask) {
        printf("Error: Position %c%d is already empty\n", 'A' + col, row + 1);
        return false;
      }
      
      printf("Black removes stone at %c%d\n", 'A' + col, row + 1);
      
      board->black &= ~mask;
      board->occupied &= ~mask;
      board->empty |= mask;
      
      return true;
      
  } else {
    if (!is_valid_initial_removal(board, row, col, false)) {
      printf("White can only remove stones adjacent to black's removal\n");
      return false;
    }

    if (!(board->white & mask)) {
      printf("Error: No white stone at %c%d\n", 'A' + col, row + 1);
      return false;
    }
    
    if (board->empty & mask) {
      printf("Error: Position %c%d is already empty\n", 'A' + col, row + 1);
      return false;
    }
    
    printf("White removes stone at %c%d\n", 'A' + col, row + 1);
    
    board->white &= ~mask;
    board->occupied &= ~mask;
    board->empty |= mask;
    
    return true;
  }
}

bool is_valid_initial_removal(const Board *board, int row, int col, bool is_black) {
  if (!board) return false;

  if (!is_valid_position(row, col)) return false;

  Bitboard mask = get_bitmask(row, col);
  if (!mask) return false;

  if (is_black) {
    bool valid = false;

    if ((row == 0 && col == 0) ||
        (row == 0 && col == BOARD_SIZE - 1) ||
        (row == BOARD_SIZE - 1 && col == 0) ||
        (row == BOARD_SIZE - 1 && col == BOARD_SIZE - 1)) {
      valid = true;      
    }

    int center = BOARD_SIZE / 2;
    if (row == center && col == center) valid = true;

    if (!valid) return false;
    if (!(board->black & mask)) return false;
    if (board->empty & mask) return false;

    return true;
  } else {
    int empty_count = popcount(board->empty);
    if (empty_count != 1) return false;

    int black_idx = __builtin_ctz(board->empty);
    int black_row, black_col;
    index_to_coord(black_idx, &black_row, &black_col);

    int row_diff = abs(row - black_row);
    int col_diff = abs(col - black_col);

    if (!((row == black_row && col_diff == 1) ||
          (col == black_col && row_diff == 1))) return false;
    
    if (!(board->white & mask)) return false;
    if (board->empty & mask) return false;
    
    return true;
  }
}

bool is_valid_move(const Board *board, Move move, bool is_white_turn) {
    if (!board) {
        return false;
    }
    
    if (IS_INITIAL_REMOVAL(move)) {
        int idx = INITIAL_REMOVAL_POS(move);
        int row, col;
        index_to_coord(idx, &row, &col);
        return is_valid_initial_removal(board, row, col, !is_white_turn);
    }

    int from_idx = MOVE_FROM(move);
    int to_idx = MOVE_TO(move);
    int captured_idx = MOVE_CAPTURED(move);
    
    int from_row, from_col, to_row, to_col, captured_row, captured_col;
    index_to_coord(from_idx, &from_row, &from_col);
    index_to_coord(to_idx, &to_row, &to_col);
    index_to_coord(captured_idx, &captured_row, &captured_col);

    if (!is_valid_position(from_row, from_col) ||
        !is_valid_position(to_row, to_col) ||
        !is_valid_position(captured_row, captured_col)) {
        return false;
    }

    Bitboard from_mask = get_bitmask(from_row, from_col);
    Bitboard to_mask = get_bitmask(to_row, to_col);
    Bitboard captured_mask = get_bitmask(captured_row, captured_col);
    
    if (!from_mask || !to_mask || !captured_mask) {
        return false;
    }
    
    if (is_white_turn) {
        if (!(board->white & from_mask)) {
            return false;
        }
    } else {
        if (!(board->black & from_mask)) {
            return false;
        }
    }
  
    if (is_white_turn) {
        if (!(board->black & captured_mask)) {
            return false;
        }
    } else {
        if (!(board->white & captured_mask)) {
            return false;
        }
    }
    
    if (!(board->empty & to_mask)) {
        return false;
    }
  
    int row_diff = to_row - from_row;
    int col_diff = to_col - from_col;
    
    bool valid_distance = false;
    if ((abs(row_diff) == 2 && col_diff == 0) ||  
        (abs(col_diff) == 2 && row_diff == 0)) {
        valid_distance = true;
    }
    
    if (!valid_distance) {
        return false;
    }
  
    int expected_captured_row = from_row + row_diff / 2;
    int expected_captured_col = from_col + col_diff / 2;
    
    if (expected_captured_row != captured_row || 
        expected_captured_col != captured_col) {
        return false;
    }
    
    // TODO: CHECK IF MULTI-JUMP IS AVAILABLE BUT NOT USED

    if (popcount(board->empty) < 2) {
        return false;
    }
    
    return true;
}

void print_move(Move move) {
  if (IS_INITIAL_REMOVAL(move)) {
    int idx = INITIAL_REMOVAL_POS(move);
    int row, col;
    index_to_coord(idx, &row, &col);

    printf("Remove stone at %c%d\n", 'A' + col, row + 1);
    return;
  }

  int from_idx = MOVE_FROM(move);
  int to_idx = MOVE_TO(move);

  int from_row, from_col, to_row, to_col;
  index_to_coord(from_idx, &from_row, &from_col);
  index_to_coord(to_idx, &to_row, &to_col);

  const char *dir_names[] = {"up", "right", "down", "left"};
  int dir = MOVE_DIRECTION(move);

  printf("%c%d -> %c%d (%s)\n",
      'A' + from_col, from_row + 1,
      'A' + to_col, to_row + 1,
      dir_names[dir]);
}

void print_move_list(MoveList *list) {
  if (!list->head) return;

  MoveNode *current = list->head;
  while (current) {
    print_move(current->move);
    current = current->next;
  }
}

char *move_to_string(Move move) {
  static char buffer[32];

  if (IS_INITIAL_REMOVAL(move)) {
    int idx = INITIAL_REMOVAL_POS(move);
    int row, col;
    index_to_coord(idx, &row, &col);
    snprintf(buffer, sizeof(buffer), "Remove %c%d",
             'A' + col, row + 1);
  } else {
    int from_idx = MOVE_FROM(move);
    int to_idx = MOVE_TO(move);

    int from_row, from_col, to_row, to_col;
    index_to_coord(from_idx, &from_row, &from_col);
    index_to_coord(to_idx, &to_row, &to_col);

    snprintf(buffer, sizeof(buffer), "%c%d-%c%d",
             'A' + from_col, from_row + 1,
             'A' + to_col, to_row + 1);
  }

  return buffer;
}
