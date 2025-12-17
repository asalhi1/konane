#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>
#include <stdbool.h>

#define BOARD_SIZE 7
#define TOTAL_CELLS (BOARD_SIZE * BOARD_SIZE)

typedef uint64_t Bitboard;

typedef struct {
  Bitboard white;
  Bitboard black;
  Bitboard occupied; // (white | black)
  Bitboard empty; // (~occupied & VALID_MASK)
} Board;

#define VALID_MASK 0x1FFFFFFFFFFFF // 2^49 - 1

int coord_to_index(int row, int col);
void index_to_coord(int index, int *row, int *col);

void init_board(Board *board);
void reset_board(Board *board);

bool is_white(const Board *board, int row, int col);
bool is_black(const Board *board, int row, int col);
bool is_empty(const Board *board, int row, int col);
void set_white(Board *board, int row, int col);
void set_black(Board *board, int row, int col);
void remove_stone(Board *board, int row, int col);

Bitboard get_bitmask(int row, int col);
void print_board(const Board *board);
Bitboard get_all_white_moves(const Board *board);
Bitboard get_all_black_moves(const Board *board);


#endif

