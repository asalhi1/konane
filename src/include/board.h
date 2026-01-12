#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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

// Check if row,col is a valid position in the 7x7 board
bool is_valid_position(int row, int col);

// Count number of on bits
int popcount(Bitboard bitboard);
// Pop the least significant bit
int pop_lsb(Bitboard *bitboard);

// Conversion functions
int coord_to_index(int row, int col);
void index_to_coord(int index, int *row, int *col);

// Initialize board
void init_board(Board *board);
void reset_board(Board *board);

// Stone manipulation/checks
bool is_white(const Board *board, int row, int col);
bool is_black(const Board *board, int row, int col);
bool is_empty(const Board *board, int row, int col);
void set_white(Board *board, int row, int col);
void set_black(Board *board, int row, int col);
void remove_stone(Board *board, int row, int col);

// Get bitmask for a position (row, col)
Bitboard get_bitmask(int row, int col);

// Print board/bitboard
void print_board(const Board *board);
void print_bitboard(const Bitboard bitboard);

#endif

