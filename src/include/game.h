#ifndef __GAME_H__
#define __GAME_H__

#include <stdbool.h>
#include <ctype.h>
#include "board.h"
#include "move.h"

void run_game(void);

bool read_coord(int *row, int *col);

bool human_choose_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq);

#endif
