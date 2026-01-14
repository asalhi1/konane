/* Board utilities and bitboard helpers.
   The 7x7 board is stored in row-major order using the lower 49 bits
   of a 64-bit bitboard (VALID_MASK). Index 0 = A1 (row 0, col 0). */
#include "board.h"

// Check if row,col is a valid position in the 7x7 board
bool is_valid_position(int row, int col) {
  if ((row < 0) || (row > BOARD_SIZE - 1) ||
      (col < 0) || (col > BOARD_SIZE - 1)) return false;

  return true;
}

// Pop the least significant bit
int pop_lsb(Bitboard *bitboard) {
  if (*bitboard == 0) return -1;

  int idx = __builtin_ctzll(*bitboard);
  *bitboard &= (*bitboard - 1);

  return idx;
}

// Count number of on bits
int popcount(Bitboard bitboard) {
  int count = 0;

  while(bitboard) count += bitboard & 1, bitboard >>= 1;

  return count;
}

// Convert coordinates to index
int coord_to_index(int row, int col) {
  return row * BOARD_SIZE + col;
}

// Convert index to coordinates
void index_to_coord(int index, int *row, int *col) {
  *row = index / BOARD_SIZE;
  *col = index % BOARD_SIZE;
}

// Initialize board
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

// Check if stone is white/black/empty
bool is_white(const Board *board, int row, int col) {
  return (board->white & get_bitmask(row, col)) != 0;
}

bool is_black(const Board *board, int row, int col) {
  return (board->black & get_bitmask(row, col)) != 0;
}

bool is_empty(const Board *board, int row, int col) {
  return (board->occupied & get_bitmask(row, col)) == 0;
}

// Set a square to white/black/remove stone
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

// Get bitmask for a position (row, col)
Bitboard get_bitmask(int row, int col) {
  if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return 0;

  int index = coord_to_index(row, col);
  
  if (index >= TOTAL_CELLS) return 0;

  return ((Bitboard)1 << index) & VALID_MASK;
}

// Print board
void print_board(const Board *board) {
  printf("  ");
  for (int col = 0; col < BOARD_SIZE; col ++) {
    printf("\033[1m%c\033[0m ", 'A' + col);  
  }
  printf("\n");

  for (int row = 0; row < BOARD_SIZE; row ++) {
    printf("\033[1m%d\033[0m ", row + 1);
    for (int col = 0; col < BOARD_SIZE; col ++) {
      if (is_white(board, row, col)) printf("\033[31mW\033[0m ");
      else if (is_black(board, row, col)) printf("\033[34mB\033[0m ");
      else printf(". ");
    }
    printf("\033[1m%d\033[0m\n", row + 1);
  }

  printf("  ");
  for (int col = 0; col < BOARD_SIZE; col ++) {
    printf("\033[1m%c\033[0m ", 'A' + col);
  }
  printf("\n");
}

// Print bitboard
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

