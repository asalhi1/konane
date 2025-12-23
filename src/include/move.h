#ifndef __MOVE_H__
#define __MOVE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "board.h"

/*
  Move encoding (single jump):
  bits  0–6   : from position
  bits  7–13  : to position
  bits 14–20  : captured position
  bits 21–22  : jump count (unused for sequences)
  bits 23–24  : direction (0=up,1=right,2=down,3=left)
*/

typedef uint32_t Move;

#define MOVE_ENCODE(from, to, captured, jump_count, dir) \
  ((Move)((from) | ((to) << 7) | ((captured) << 14) | \
          ((jump_count) << 21) | ((dir) << 23)))

#define MOVE_FROM(move)       ((move) & 0x7F)
#define MOVE_TO(move)         (((move) >> 7) & 0x7F)
#define MOVE_CAPTURED(move)   (((move) >> 14) & 0x7F)
#define MOVE_JUMP_COUNT(move) (((move) >> 21) & 0x3)
#define MOVE_DIRECTION(move)  (((move) >> 23) & 0x3)

#define MOVE_INITIAL_REMOVAL_ENCODE(pos) (0x80000000 | (pos))
#define IS_INITIAL_REMOVAL(move)         ((move) & 0x80000000)
#define INITIAL_REMOVAL_POS(move)        ((move) & 0x7F)

#define MAX_MOVES 32

typedef struct {
  int count;
  Move jumps[MAX_MOVES];
} MoveSequence;

Move create_simple_jump(int from_row, int from_col,
                        int to_row, int to_col,
                        int captured_row, int captured_col,
                        int direction);

Move create_initial_removal(int row, int col);

int generate_all_moves(const Board *board,
                       bool is_white_turn,
                       MoveSequence *out_moves);

void dfs_jumps(const Board *board,
               int row, int col,
               bool is_white_turn,
               MoveSequence *current,
               MoveSequence *results,
               int *result_count);

bool execute_sequence(Board *board,
                      const MoveSequence *seq,
                      bool is_white_turn);

bool execute_initial_removal(Board *board,
                             int row, int col,
                             bool is_black);

bool is_valid_initial_removal(const Board *board,
                              int row, int col,
                              bool is_black);

#endif
