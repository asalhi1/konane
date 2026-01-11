#ifndef __AI_H__
#define __AI_H__

#include "board.h"
#include "game.h"
#include "move.h"
#include <stdlib.h>
#include <time.h>

#define MOBILITY_WEIGHT 20
#define MATERIAL_WEIGHT 15
#define CORNER_WEIGHT 6
#define EDGE_WEIGHT 3
#define JUMP_POTENTIAL_WEIGHT 5
#define ISOLATION_PENALTY -3

#define INT_MIN -1e9
#define INT_MAX 1e9

#define DEF_DEPTH 5

void ai_init(void);

bool ai_random_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq);

int eval_position(Board *board, bool is_white);
int negamax(Board *board, int depth, bool is_white, int alpha, int beta, MoveSequence *best_sequence);
bool get_best_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq, int depth);

#endif
