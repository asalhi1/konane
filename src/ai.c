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
