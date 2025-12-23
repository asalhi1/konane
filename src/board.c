#include "board.h"

bool is_valid_position(int row, int col) {
  if ((row < 0) || (row > BOARD_SIZE - 1) ||
      (col < 0) || (col > BOARD_SIZE - 1)) return false;

  return true;
}

int pop_lsb(Bitboard *bitboard) {
  if (*bitboard == 0) return -1;

  int idx = __builtin_ctzll(*bitboard);
  *bitboard &= (*bitboard - 1);

  return idx;
}

int popcount(Bitboard bitboard) {
  int count = 0;

  while(bitboard) count += bitboard & 1, bitboard >>= 1;

  return count;
}

int coord_to_index(int row, int col) {
  return row * BOARD_SIZE + col;
}

void index_to_coord(int index, int *row, int *col) {
  *row = index / BOARD_SIZE;
  *col = index % BOARD_SIZE;
}

void init_board(Board *board) {
  board->white = 0;
  board->black = 0;
  board->occupied = 0;
  board->empty = VALID_MASK;

  for (int row = 0; row < BOARD_SIZE; row ++) {
    for (int col = 0; col < BOARD_SIZE; col ++) {
      if ((row + col) % 2) board->white |= get_bitmask(row, col);
      else board->black |= get_bitmask(row, col);
    }
  }

  board->occupied = board->white | board->black;
  board->empty = (~board->occupied) & VALID_MASK;
}

bool is_white(const Board *board, int row, int col) {
  return (board->white & get_bitmask(row, col)) != 0;
}

bool is_black(const Board *board, int row, int col) {
  return (board->black & get_bitmask(row, col)) != 0;
}

bool is_empty(const Board *board, int row, int col) {
  return (board->occupied & get_bitmask(row, col)) == 0;
}

void set_white(Board *board, int row, int col) {
  Bitboard mask = get_bitmask(row, col);
  board->white |= mask;
  board->occupied |= mask;
  board->empty &= ~mask;
}

void set_black(Board *board, int row, int col) {
  Bitboard mask = get_bitmask(row, col);
  board->black |= mask;
  board->occupied |= mask;
  board->empty &= ~mask;
}

void remove_stone(Board *board, int row, int col) {
  Bitboard mask = get_bitmask(row, col);
  board->occupied &= ~mask;
  board->white &= ~mask;
  board->black &= ~mask;
  board->empty |= mask;
}

Bitboard get_bitmask(int row, int col) {
  if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return 0;

  int index = coord_to_index(row, col);
  
  if (index >= TOTAL_CELLS) return 0;

  return ((Bitboard)1 << index) & VALID_MASK;
}

void print_board(const Board *board) {
  printf("  ");
  for (int col = 0; col < BOARD_SIZE; col ++) {
    printf("%c ", 'A' + col);  
  }
  printf("\n");

  for (int row = 0; row < BOARD_SIZE; row ++) {
    printf("%d ", row + 1);
    for (int col = 0; col < BOARD_SIZE; col ++) {
      if (is_white(board, row, col)) printf("W ");
      else if (is_black(board, row, col)) printf("B ");
      else printf(". ");
    }
    printf("%d\n", row + 1);
  }

  printf("  ");
  for (int col = 0; col < BOARD_SIZE; col ++) {
    printf("%c ", 'A' + col);
  }
  printf("\n");
}

void print_bitboard(const Bitboard bitboard) {
  printf("  ");
  for (int col = 0; col < BOARD_SIZE; col ++) {
    printf("%c ", 'A' + col);
  }
  printf("\n");

  for (int row = 0; row < BOARD_SIZE; row ++) {
    printf("%d ", row + 1);
    for (int col = 0; col < BOARD_SIZE; col ++) {
      Bitboard mask = get_bitmask(row, col);
      printf("%c ", (bitboard & mask) ? '1' : '0');
    }
    printf("%d\n", row + 1);
  }
  
  printf("  ");
  for (int col = 0; col < BOARD_SIZE; col ++) {
    printf("%c ", 'A' + col);
  }
  printf("\n");
}

