#include "perft.h"
#include "ai.h"
#include "move.h"
#include "board.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

// Recursive perft node counter
static uint64_t perft_nodes_internal(Board *board, bool is_white_turn, int depth) {
  if (depth == 0) return 1;

  MoveSequence moves[MAX_SEQUENCES];
  int num_moves = generate_all_moves(board, is_white_turn, moves);
  if (num_moves == 0) return 1; // leaf: no moves

  uint64_t total = 0;
  for (int i = 0; i < num_moves; i++) {
    Board bcopy = *board;
    if (!execute_sequence(&bcopy, &moves[i], is_white_turn)) continue;
    total += perft_nodes_internal(&bcopy, !is_white_turn, depth - 1);
  }
  return total;
}

uint64_t perft_nodes(const Board *board, bool is_white_turn, int depth) {
  if (!board || depth < 0) return 0;
  Board bcopy = *board;
  return perft_nodes_internal(&bcopy, is_white_turn, depth);
}

void perft_divide(const Board *board, bool is_white_turn, int depth) {
  if (!board || depth <= 0) {
    printf("Invalid arguments to perft_divide\n");
    return;
  }

  MoveSequence moves[MAX_SEQUENCES];
  int num_moves = generate_all_moves(board, is_white_turn, moves);

  uint64_t total = 0;
  for (int i = 0; i < num_moves; i++) {
    Board bcopy = *board;
    if (!execute_sequence(&bcopy, &moves[i], is_white_turn)) continue;
    uint64_t cnt = perft_nodes_internal(&bcopy, !is_white_turn, depth - 1);
    printf("Move %2d: ", i + 1);
    print_move_sequence(&moves[i]);
    printf(" -> %llu\n", (unsigned long long)cnt);
    total += cnt;
  }
  printf("Total nodes at depth %d: %llu\n", depth, (unsigned long long)total);
}

// Similar to negamax but increments node_counter on each call */
static int negamax_count(Board *board, int depth, bool is_white, int alpha, int beta, uint64_t *node_counter) {
  if (node_counter) (*node_counter)++;

  if (depth == 0) {
    return eval_position(board, is_white);
  }

  MoveSequence moves[MAX_SEQUENCES];
  for (int i = 0; i < MAX_SEQUENCES; i++) moves[i].count = 0;

  int num_moves = generate_all_moves(board, is_white, moves);
  if (num_moves == 0) return -10000 + depth;

  int best_score = -1000000000;

  for (int i = 0; i < num_moves; i++) {
    Board bcopy = *board;
    if (!execute_sequence(&bcopy, &moves[i], is_white)) continue;
    int score = -negamax_count(&bcopy, depth - 1, !is_white, -beta, -alpha, node_counter);
    if (score > best_score) best_score = score;
    if (score > alpha) alpha = score;
    if (alpha >= beta) break;
  }

  return best_score;
}

// Benchmark wrapper: runs the negamax repeatedly and optionally counts nodes using negamax_count
double perft_benchmark_negamax(const Board *board, bool is_white_turn, int depth, int iterations, uint64_t *nodes_out) {
  if (!board || depth <= 0 || iterations <= 0) return 0.0;

  clock_t start = clock();
  for (int i = 0; i < iterations; i++) {
    negamax((Board *)board, depth, is_white_turn, INT_MIN, INT_MAX, NULL);
  }
  clock_t end = clock();
  double elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;

  if (nodes_out) {
    uint64_t nodes = 0;
    Board bcopy = *board;
    (void)negamax_count(&bcopy, depth, is_white_turn, INT_MIN, INT_MAX, &nodes);
    *nodes_out = nodes;
  }

  return elapsed_seconds;
}

// A small interactive menu to trigger tests and presets
void perft_menu(void) {
  Board board;
  init_board(&board);

  while (1) {
    printf("\n-- Perft / Performance Tests --\n");
    printf("(1) Perft nodes (single depth)\n");
    printf("(2) Perft divide (show per-move counts)\n");
    printf("(3) Negamax benchmark (time multiple runs)\n");
    printf("(4) Run preset suite (depths 1..4)\n");
    printf("(0) Back\n");
    printf("> ");

    int cmd = -1;
    if (scanf("%d", &cmd) != 1) break;

    if (cmd == 0) break;

    if (cmd == 1) {
      int depth = 1;
      printf("Depth: "); scanf("%d", &depth);
      uint64_t nodes = perft_nodes(&board, true, depth);
      printf("Perft nodes (depth %d): %llu\n", depth, (unsigned long long)nodes);
    } else if (cmd == 2) {
      int depth = 2;
      printf("Depth: "); scanf("%d", &depth);
      perft_divide(&board, true, depth);
    } else if (cmd == 3) {
      int depth = 3;
      int iterations = 10;
      printf("Depth: "); scanf("%d", &depth);
      printf("Iterations: "); scanf("%d", &iterations);
      uint64_t nodes = 0;
      double t = perft_benchmark_negamax(&board, true, depth, iterations, &nodes);
      printf("Negamax benchmark: iterations=%d, depth=%d, total_time=%.4fs, avg=%.6fs, nodes=%llu\n",
             iterations, depth, t, t / iterations, (unsigned long long)nodes);
    } else if (cmd == 4) {
      printf("Running preset suite (depths 1..4) as white:\n");
      for (int d = 1; d <= 4; d++) {
        clock_t s = clock();
        uint64_t n = perft_nodes(&board, true, d);
        clock_t e = clock();
        double secs = (double)(e - s) / CLOCKS_PER_SEC;
        printf(" depth %d: %llu nodes, time=%.4fs\n", d, (unsigned long long)n, secs);
      }
    } else {
      printf("Unknown command\n");
    }
  }
}
