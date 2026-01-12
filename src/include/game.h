#ifndef __GAME_H__
#define __GAME_H__

#include <stdbool.h>
#include <ctype.h>
#include "ai.h"
#include "board.h"
#include "move.h"

typedef enum {
  GAME_MODE_HUMAN_VS_HUMAN,
  GAME_MODE_HUMAN_VS_AI,
  GAME_MODE_AI_VS_AI
} GameMode;

typedef enum {
  PLAYER_TYPE_HUMAN,
  PLAYER_TYPE_AI_RANDOM
} PlayerType;


// Main game loop
void run_game(GameMode mode);

// Read coordinate from user
bool read_coord(int *row, int *col);

// Let human player choose a move for a side
bool human_choose_move(Board *board, bool is_white_turn, MoveSequence *chosen_seq);
// Let AI player choose a move for a side
bool play_ai_turn(Board *board, bool is_white_turn, MoveSequence *chosen_seq);

#endif
