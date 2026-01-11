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

int eval_position(Board *board, bool is_white) {
  int score = 0;

  MoveSequence white_moves[MAX_MOVES];
  MoveSequence black_moves[MAX_MOVES];

  // Mobility
  int white_mob = generate_all_moves(board, true, white_moves);
  int black_mob = generate_all_moves(board, false, black_moves);

  int mob_diff = white_mob - black_mob;

  if (is_white) score += mob_diff * MOBILITY_WEIGHT;
  else score -= mob_diff * MOBILITY_WEIGHT;

  // Material
  int white_stones = popcount(board->white);
  int black_stones = popcount(board->black);

  int stone_diff = white_stones - black_stones;

  if (is_white) score += stone_diff * MATERIAL_WEIGHT;
  else score -= stone_diff * MATERIAL_WEIGHT;

  // Endgame
  if (white_mob == 0 && black_mob > 0) {
    return is_white ? -10000 : 10000;
  }

  if (black_mob == 0 && white_mob > 0) {
    return is_white ? 10000 : -10000;
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

  printf("Negamax: depth %d, score %d\n", depth, score);
  return true;
}
