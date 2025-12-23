#ifndef __MOVE_H__
#define __MOVE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "board.h"

// Move encoding in a 32 bit integer:
// 7 bits (0-6) for from position
// 7 bits (7-13) for to position
// 7 bits (14-20) for captured position
// 2 bits (21-22) for jump count
// 2 bits (23-24) for direction (0=up, 1=right, 2=down, 3=left)
// 7 bits (25-31) for flags

typedef uint32_t Move;

#define MOVE_ENCODE(from, to, captured, jump_count, dir) \
  ((Move)((from) | ((to) << 7) ) | ((captured) << 14) | \
  ((jump_count) << 21) | ((dir) << 23))

#define MOVE_FROM(move)       ((move) & 0x7F)
#define MOVE_TO(move)         (((move) >> 7) & 0x7F)
#define MOVE_CAPTURED(move)   (((move) >> 14) & 0x7F)
#define MOVE_JUMP_COUNT(move) (((move) >> 21) & 0x3)
#define MOVE_DIRECTION(move)  (((move) >> 23) & 0x3)

// Initial stone removal (doesn't count as jump)
#define MOVE_INITIAL_REMOVAL_ENCODE(pos) (0x80000000 | (pos))
#define IS_INITIAL_REMOVAL(move) ((move) & 0x80000000)
#define INITIAL_REMOVAL_POS(move) ((move) & 0x7F)

typedef struct MoveNode {
  Move move;
  struct MoveNode *next;
} MoveNode;

typedef struct {
  MoveNode *head;
  MoveNode *tail;
  int count;
} MoveList;

Move create_simple_jump(int from_row, int from_col,
                        int to_row, int to_col,
                        int captured_row, int captured_col,
                        int direction);
Move create_initial_removal(int row, int col);

MoveList *create_move_list();
void free_move_list(MoveList *list);
void add_move(MoveList *list, Move move);
void add_move_coords(MoveList *list, int from_row, int from_col,
                     int to_row, int to_col,
                     int captured_row, int captured_col,
                     int direction);
Move pop_move(MoveList *list);

MoveList *generate_all_moves(const Board *board, bool is_white_turn);
MoveList *generate_moves_for_stone(const Board *board, 
                                   int from_row, int from_col,
                                   bool is_white_turn);
MoveList *generate_initial_removals(const Board *board, bool is_black);

bool execute_move(Board *board, Move move, bool is_white_turn);
bool execute_initial_removal(Board *board, int row, int col, bool is_black);

bool is_valid_move(const Board *board, Move move, bool is_white_turn);
bool is_valid_initial_removal(const Board *board, int row, int col, bool is_black);

void print_move(Move move);
void print_move_list(MoveList *list);

char *move_to_string(Move move);

#endif

