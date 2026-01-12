#ifndef __AI_H__
#define __AI_H__

#include "board.h"
#include "game.h"
#include "move.h"
#include <stdlib.h>
#include <time.h>

// Define weights for different eval factors
#define MOBILITY_WEIGHT 20
#define MATERIAL_WEIGHT 15
#define CORNER_WEIGHT 6
#define EDGE_WEIGHT 3
#define JUMP_POTENTIAL_WEIGHT 5
#define ISOLATION_PENALTY -3

// Alpha/Beta minimums and maximums
#define INT_MIN -1e9
#define INT_MAX 1e9

// Default depth
#define DEF_DEPTH 5

// Seed rand
void ai_init(void);

// Choose a random valid move
bool ai_random_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq);

// Evaluate a position from a player's perspective
int eval_position(Board *board, bool player_is_white);
// Negamax search with alpha beta pruning
int negamax(Board *board, int depth, bool is_white, int alpha, int beta, MoveSequence *best_sequence);
// Wrapper to get best move
bool get_best_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq, int depth);

#endif
