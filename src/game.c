#include "game.h"

void run_game(GameMode mode) {
    if (mode == GAME_MODE_HUMAN_VS_AI || mode == GAME_MODE_AI_VS_AI) {
        ai_init();
    }
    
    Board *board = (Board *)malloc(sizeof(Board));
    init_board(board);
  
    bool human_is_black = true;
    bool human_is_white = false;
    
    if (mode == GAME_MODE_HUMAN_VS_AI) {
        human_is_black = (rand() % 2) == 0;
        human_is_white = !human_is_black;
        
        printf("Human plays as: %s\n", human_is_black ? "Black (starts)" : "White");
        printf("AI plays as: %s\n", human_is_black ? "White" : "Black (starts)");
    } else if (mode == GAME_MODE_AI_VS_AI) {
        printf("AI 1: Black (starts)\n");
        printf("AI 2: White\n");
    }
    
    print_board(board);

    int row, col;
    
    if (mode == GAME_MODE_HUMAN_VS_HUMAN || 
        (mode == GAME_MODE_HUMAN_VS_AI && human_is_black)) {
        do {
            printf("Black: remove a stone (corner/center): ");
        } while (!read_coord(&row, &col) || !execute_initial_removal(board, row, col, true));
    } else {
        printf("Black (AI) is removing a stone...\n");
        
        int valid_positions[5][2] = {
            {0, 0},     
            {0, BOARD_SIZE - 1},
            {BOARD_SIZE - 1, 0},
            {BOARD_SIZE - 1, BOARD_SIZE - 1},
            {BOARD_SIZE / 2, BOARD_SIZE / 2}
        };
        
        int chosen_index = rand() % 5;
        row = valid_positions[chosen_index][0];
        col = valid_positions[chosen_index][1];
        
        if (!execute_initial_removal(board, row, col, true)) {
            for (int i = 0; i < 5; i++) {
                row = valid_positions[i][0];
                col = valid_positions[i][1];
                if (execute_initial_removal(board, row, col, true)) {
                    break;
                }
            }
        }
        
        printf("Black removed stone at %c%d\n", 'A' + col, row + 1);
    }
    
    print_board(board);
    
    if (mode == GAME_MODE_HUMAN_VS_HUMAN || 
        (mode == GAME_MODE_HUMAN_VS_AI && human_is_white)) {
        do {
            printf("White: remove a stone adjacent to black's removal: ");
        } while (!read_coord(&row, &col) || !execute_initial_removal(board, row, col, false));
    } else {
        printf("White (AI) is removing a stone...\n");
        
        int attempts = 0;
        do {
            row = rand() % BOARD_SIZE;
            col = rand() % BOARD_SIZE;
            attempts++;
  
            if (execute_initial_removal(board, row, col, false)) {
                printf("White removed stone at %c%d\n", 'A' + col, row + 1);
                break;
            }
            
            if (attempts > 100) {
                printf("AI couldn't find valid white removal. Using fallback.\n");
                execute_initial_removal(board, 0, 1, false);
                break;
            }
        } while (true);
    }
    
    print_board(board);
    
    bool is_white_turn = false;
    int move_count = 0;

    while (true) {
        printf("--- Move %d ---\n", ++move_count);
        print_board(board);
        
        MoveSequence seq;
        bool has_move = false;

        bool is_human_turn = false;
        
        if (mode == GAME_MODE_HUMAN_VS_HUMAN) {
            is_human_turn = true;
        } else if (mode == GAME_MODE_HUMAN_VS_AI) {
            is_human_turn = (is_white_turn && human_is_white) || 
                           (!is_white_turn && human_is_black);
        }

        if (is_human_turn) {
            has_move = human_choose_move(board, is_white_turn, &seq);
            
            if (!has_move) {
                printf("%s has no legal moves!\n", 
                       is_white_turn ? "White" : "Black");
                break;
            }
        } else {
            printf("%s (AI) is thinking...\n", 
                   is_white_turn ? "White" : "Black");
            
            has_move = get_best_move(board, is_white_turn, &seq, DEF_DEPTH);
            
            if (!has_move) {
                printf("%s (AI) has no legal moves!\n", 
                       is_white_turn ? "White" : "Black");
                break;
            }
            
            printf("AI plays: ");
            print_move_sequence(&seq);
        }
        
        execute_sequence(board, &seq, is_white_turn);

        is_white_turn = !is_white_turn;
        
        if (mode == GAME_MODE_AI_VS_AI) {
            printf("Press Enter to continue...");
            while (getchar() != '\n');
        }
    }
    
    printf("\n=== GAME OVER ===\n");
    print_board(board);
  
    printf("Winner: %s\n", 
           is_white_turn ? "Black" : "White");
    
    free(board);
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
