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
      printf("Coordinates out of bounds! Board is %dx%d.\n", BOARD_SIZE, BOARD_SIZE);
      continue;
    }

    int from_idx = coord_to_index(from_r, from_col);
    int to_idx   = coord_to_index(to_r, to_col);

    MoveSequence matching_sequences[MAX_MOVES];
    int num_matching = 0;
    
    for (int i = 0; i < num_moves; i++) {
      if (all_moves[i].count == 0)
        continue;
      
      if (MOVE_FROM(all_moves[i].jumps[0]) == (unsigned int)from_idx &&
        MOVE_TO(all_moves[i].jumps[all_moves[i].count - 1]) == (unsigned int)to_idx) {
        matching_sequences[num_matching++] = all_moves[i];
      }
    }

    if (num_matching == 0) {
      printf("No valid move from %c%d to %c%d\n", 
              from_col_char, from_row, to_col_char, to_row);
      continue;
    }

    if (num_matching == 1) {
      *chosen_seq = matching_sequences[0];
      return true;
    }

    printf("\nFound %d different capture sequences from %c%d to %c%d:\n",
            num_matching, from_col_char, from_row, to_col_char, to_row);
    
    for (int i = 0; i < num_matching; i++) {
      printf("%d. ", i + 1);
      print_move_sequence(&matching_sequences[i]);
    }
    
    printf("Choose sequence (1-%d): ", num_matching);
    
    int choice;
    if (scanf("%d", &choice) != 1) {
      printf("Invalid choice!\n");
      while (getchar() != '\n');
      continue;
    }
    
    if (choice < 1 || choice > num_matching) {
      printf("Choice must be between 1 and %d!\n", num_matching);
      continue;
    }
    
    *chosen_seq = matching_sequences[choice - 1];
    return true;
  }
}
