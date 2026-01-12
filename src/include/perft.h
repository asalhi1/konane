#ifndef __PERFT_H__
#define __PERFT_H__

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

uint64_t perft_nodes(const Board *board, bool is_white_turn, int depth);
void perft_divide(const Board *board, bool is_white_turn, int depth);

/* Run a simple negamax benchmark: returns total time in seconds.
 * If nodes_out != NULL, fills in the node count measured by a counting negamax run.
 */
double perft_benchmark_negamax(const Board *board, bool is_white_turn, int depth, int iterations, uint64_t *nodes_out);

// Interactive menu for running perf tests 
void perft_menu(void);

#endif
