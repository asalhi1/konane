#ifndef PERFT_H
#define PERFT_H

#include "board.h"
#include <stdint.h>
#include <stdbool.h>

// Node counting
uint64_t perft_nodes(const Board *board, bool is_white_turn, int depth);
void perft_divide(const Board *board, bool is_white_turn, int depth);

// Benchmarking
double perft_benchmark_negamax(const Board *board, bool is_white_turn, 
                              int depth, int iterations, uint64_t *nodes_out);

// Testing
void perft_test_suite(void);
void perft_diagnostic(void);
void perft_menu(void);

#endif
