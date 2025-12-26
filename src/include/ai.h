#ifndef __AI_H__
#define __AI_H__

#include "board.h"
#include "game.h"
#include "move.h"
#include <stdlib.h>
#include <time.h>

void ai_init(void);

bool ai_random_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq);

#endif
