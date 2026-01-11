#include "ai.h"

void ai_init(void) {
  srand(time(NULL));
}

bool ai_random_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq) {
  MoveSequence all_moves[MAX_MOVES];
  int num_moves = generate_all_moves(board, is_white_turn, all_moves);

  if (num_moves == 0) return false;

  int rand_idx = rand() % num_moves;
  *chosen_seq = all_moves[rand_idx];

  return true;
}

/* Direction vectors: up, right, down, left */
static const int dir_row[4] = {-1, 0, 1, 0};
static const int dir_col[4] = {0, 1, 0, -1};

int eval_position(Board *board, bool player_is_white) {  
  int score = 0;

  MoveSequence white_moves[MAX_MOVES];
  MoveSequence black_moves[MAX_MOVES];

  // Mobility
  int white_mob = generate_all_moves(board, true, white_moves);
  int black_mob = generate_all_moves(board, false, black_moves);

  int mob_diff = white_mob - black_mob;

  if (player_is_white) score += mob_diff * MOBILITY_WEIGHT;  
  else score -= mob_diff * MOBILITY_WEIGHT;  

  // Material
  int white_stone_count = popcount(board->white);
  int black_stone_count = popcount(board->black);

  int stone_diff = white_stone_count - black_stone_count;

  if (player_is_white) score += stone_diff * MATERIAL_WEIGHT;  
  else score -= stone_diff * MATERIAL_WEIGHT;  

  // Corners
  Bitboard corner_mask = get_bitmask(0, 0) | get_bitmask(0, BOARD_SIZE-1) |
                         get_bitmask(BOARD_SIZE-1, 0) | get_bitmask(BOARD_SIZE-1, BOARD_SIZE-1);

  int white_corners = popcount(board->white & corner_mask);
  int black_corners = popcount(board->black & corner_mask);
  int corner_diff = white_corners - black_corners;

  if (player_is_white) score += corner_diff * CORNER_WEIGHT;  
  else score -= corner_diff * CORNER_WEIGHT;  

  // Edges
  Bitboard edge_mask = 0;
  for (int i = 0; i < BOARD_SIZE; i++) {
    edge_mask |= get_bitmask(0, i);
    edge_mask |= get_bitmask(BOARD_SIZE-1, i);
    edge_mask |= get_bitmask(i, 0);
    edge_mask |= get_bitmask(i, BOARD_SIZE-1);
  }

  edge_mask &= ~corner_mask;

  int white_edges = popcount(board->white & edge_mask);
  int black_edges = popcount(board->black & edge_mask);
  int edge_diff = white_edges - black_edges;

  if (player_is_white) score += edge_diff * EDGE_WEIGHT;  
  else score -= edge_diff * EDGE_WEIGHT;  

  // Jump potential
  int white_jump_potential = 0;
  int black_jump_potential = 0;

  Bitboard white_stones_bb = board->white;
  while (white_stones_bb) {
    int idx = pop_lsb(&white_stones_bb);
    int row, col;
    index_to_coord(idx, &row, &col);
    
    for (int dir = 0; dir < 4; dir++) {
      int jump_row = row + dir_row[dir];
      int jump_col = col + dir_col[dir];
      int land_row = row + 2 * dir_row[dir];
      int land_col = col + 2 * dir_col[dir];
      
      if (is_valid_position(jump_row, jump_col) && 
        is_valid_position(land_row, land_col)) {
        if (is_black(board, jump_row, jump_col) &&  // This is a function call
          is_empty(board, land_row, land_col)) {
          white_jump_potential++;
        }
      }
    }
  }

  Bitboard black_stones_bb = board->black;
  while (black_stones_bb) {
    int idx = pop_lsb(&black_stones_bb);
    int row, col;
    index_to_coord(idx, &row, &col);
    
    for (int dir = 0; dir < 4; dir++) {
      int jump_row = row + dir_row[dir];
      int jump_col = col + dir_col[dir];
      int land_row = row + 2 * dir_row[dir];
      int land_col = col + 2 * dir_col[dir];
      
      if (is_valid_position(jump_row, jump_col) && 
          is_valid_position(land_row, land_col)) {
        if (is_white(board, jump_row, jump_col) &&  // This is a function call
          is_empty(board, land_row, land_col)) {
          black_jump_potential++;
        }
      }
    }
  }

  int jump_pot_diff = white_jump_potential - black_jump_potential;
  if (player_is_white) score += jump_pot_diff * JUMP_POTENTIAL_WEIGHT;  
  else score -= jump_pot_diff * JUMP_POTENTIAL_WEIGHT;  

  // Isolated stones (penalty)
  int white_isolated = 0;
  int black_isolated = 0;

  // Check each white stone
  Bitboard stones_bb = board->white;
  while (stones_bb) {
    int idx = pop_lsb(&stones_bb);
    int row, col;
    index_to_coord(idx, &row, &col);
    
    bool has_friendly_neighbor = false;
    for (int dir = 0; dir < 4; dir++) {
      int n_row = row + dir_row[dir];
      int n_col = col + dir_col[dir];
      if (is_valid_position(n_row, n_col) && is_white(board, n_row, n_col)) {  // Function call
        has_friendly_neighbor = true;
        break;
      }
    }
    if (!has_friendly_neighbor) white_isolated++;
  }

  // Check each black stone
  stones_bb = board->black;
  while (stones_bb) {
    int idx = pop_lsb(&stones_bb);
    int row, col;
    index_to_coord(idx, &row, &col);
    
    bool has_friendly_neighbor = false;
    for (int dir = 0; dir < 4; dir++) {
      int n_row = row + dir_row[dir];
      int n_col = col + dir_col[dir];
      if (is_valid_position(n_row, n_col) && is_black(board, n_row, n_col)) {  // Function call
        has_friendly_neighbor = true;
        break;
      }
    }
    if (!has_friendly_neighbor) black_isolated++;
  }

  int isolation_diff = white_isolated - black_isolated;
  if (player_is_white) score += isolation_diff * ISOLATION_PENALTY;  
  else score -= isolation_diff * ISOLATION_PENALTY;

  // Endgame
  if (white_mob == 0 && black_mob > 0) {
    return player_is_white ? -10000 : 10000;
  }

  if (black_mob == 0 && white_mob > 0) {
    return player_is_white ? 10000 : -10000;
  }

  return score;
}

int negamax(Board *board, int depth, bool is_white, int alpha, int beta, MoveSequence *best_sequence) {
    //static int call_count = 0;
    //static int max_depth = 0;
    //call_count++;
    //if (depth > max_depth) max_depth = depth;
    
    //printf("[negamax] Call #%d, depth=%d, is_white=%d, max_depth=%d\n", 
    //       call_count, depth, is_white, max_depth);
    
    if (!board) {
        printf("[negamax] ERROR: NULL board!\n");
        return 0;
    }
    
    //printf("[negamax] Board state: white=0x%lx, black=0x%lx, empty=0x%lx\n",
    //       board->white, board->black, board->empty);
    
    if (depth == 0) {
        int eval = eval_position(board, is_white);
    //    printf("[negamax] depth=0, returning eval=%d\n", eval);
        return eval;
    }

    MoveSequence moves[MAX_MOVES];
    for (int i = 0; i < MAX_MOVES; i++) {
        moves[i].count = 0;
    }
    
    int num_moves = generate_all_moves(board, is_white, moves);
    //printf("[negamax] Generated %d moves\n", num_moves);
    
    if (num_moves == 0) {
    //    printf("[negamax] No moves, returning -10000\n");
        return -10000 + depth;
    }

    int best_score = INT_MIN;
    MoveSequence best_local_sequence = { 0 };

    for (int i = 0; i < num_moves && i < MAX_MOVES; i++) {
        //printf("[negamax] Processing move %d/%d\n", i+1, num_moves);
        
        Board board_copy;
        board_copy.white = board->white;
        board_copy.black = board->black;
        board_copy.occupied = board->occupied;
        board_copy.empty = board->empty;
        
        //printf("[negamax] Before execute: white=0x%lx\n", board_copy.white);
        
        if (!execute_sequence(&board_copy, &moves[i], is_white)) {
            //printf("[negamax] WARNING: execute_sequence failed for move %d\n", i);
            continue;
        }
        
        //printf("[negamax] After execute: white=0x%lx\n", board_copy.white);
        
        if (board_copy.white == 0 && board_copy.black == 0 && board_copy.empty == 0) {
            //printf("[negamax] CRITICAL: Board zeroed out after execute_sequence!\n");
            continue;
        }
        
        int score = -negamax(&board_copy, depth - 1, !is_white, -beta, -alpha, NULL);
        
        //printf("[negamax] Move %d score: %d\n", i+1, score);

        if (score > best_score) {
            best_score = score;
            best_local_sequence = moves[i];
        }
        
        if (score > alpha)
            alpha = score;

        if (alpha >= beta) {
            //printf("[negamax] Beta cutoff at move %d\n", i+1);
            break;
        }
    }

    if (best_local_sequence.count > 0 && best_sequence) {
        *best_sequence = best_local_sequence;
    }

    //printf("[negamax] Returning best_score=%d\n", best_score);
    return best_score;
}

bool get_best_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq, int depth) {
  if (!board || !chosen_seq || depth <= 0) return false;

  MoveSequence moves[MAX_MOVES];
  int num_moves = generate_all_moves(board, is_white_turn, moves);

  if (num_moves == 0) return false;

  if (num_moves == 1) {
    *chosen_seq = moves[0];
    return true;
  }

  MoveSequence best_sequence = { 0 };
  int alpha = INT_MIN;
  int beta = INT_MAX;

  int score = negamax(board, depth, is_white_turn, alpha, beta, &best_sequence);

  if (best_sequence.count == 0) {
    *chosen_seq = moves[0];
  } else {
    *chosen_seq = best_sequence;
  }

  printf("[Negamax] depth %d, score %d\n", depth, score);
  return true;
}
