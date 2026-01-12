#include "perft.h"
#include "ai.h"
#include "move.h"
#include "board.h"
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

// Recursive perft node counter
static uint64_t perft_nodes_internal(Board *board, bool is_white_turn, int depth, int *ply) {
  (*ply)++;
  
  if (depth == 0) {
    (*ply)--;
    return 1;  // Leaf node
  }

  MoveSequence moves[MAX_SEQUENCES];
  int num_moves = generate_all_moves(board, is_white_turn, moves);
  
  if (num_moves == 0) {
    (*ply)--;
    return 1;
  }

  uint64_t total = 0;
  for (int i = 0; i < num_moves; i++) {
    Board bcopy = *board;
    
    // Skip if move execution fails
    if (!execute_sequence(&bcopy, &moves[i], is_white_turn)) {
      printf("WARNING: Failed to execute valid move at ply %d\n", *ply);
      continue;
    }
    
    total += perft_nodes_internal(&bcopy, !is_white_turn, depth - 1, ply);
  }
  
  (*ply)--;
  return total;
}

uint64_t perft_nodes(const Board *board, bool is_white_turn, int depth) {
  if (!board || depth < 0) return 0;
  
  // Create a mutable copy
  Board bcopy = *board;
  int ply = 0;
  return perft_nodes_internal(&bcopy, is_white_turn, depth, &ply);
}

void perft_divide(const Board *board, bool is_white_turn, int depth) {
  if (!board || depth <= 0) {
    printf("Invalid arguments to perft_divide\n");
    return;
  }

  MoveSequence moves[MAX_SEQUENCES];
  int num_moves = generate_all_moves(board, is_white_turn, moves);
  
  if (num_moves == 0) {
    printf("No legal moves from this position\n");
    return;
  }

  printf("\nPerft Divide - Depth %d\n", depth);
  printf("========================\n");
  
  uint64_t total = 0;
  for (int i = 0; i < num_moves; i++) {
    Board bcopy = *board;
    
    if (!execute_sequence(&bcopy, &moves[i], is_white_turn)) {
      printf("Move %2d: [EXECUTION FAILED] ", i + 1);
      print_move_sequence(&moves[i]);
      printf("\n");
      continue;
    }
    
    uint64_t cnt = perft_nodes(&bcopy, !is_white_turn, depth - 1);
    printf("Move %2d: ", i + 1);
    print_move_sequence(&moves[i]);
    printf(" -> %llu\n", (unsigned long long)cnt);
    total += cnt;
  }
  
  printf("\nTotal nodes at depth %d: %llu\n", depth, (unsigned long long)total);
  
  // Verification: run full perft and compare
  uint64_t full_count = perft_nodes(board, is_white_turn, depth);
  if (full_count == total) {
    printf("✓ Verification passed: %llu == %llu\n", 
           (unsigned long long)full_count, (unsigned long long)total);
  } else {
    printf("✗ Verification FAILED: %llu != %llu\n", 
           (unsigned long long)full_count, (unsigned long long)total);
  }
}

// Test with known positions
void perft_test_suite(void) {
  printf("\n=== KONANE PERFT TEST SUITE ===\n");
  
  // Test 1: Initial position (after opening removals)
  printf("\nTest 1: Standard opening position\n");
  Board board1;
  init_board(&board1);
  
  // Do standard opening: Black D4, White D3
  if (!execute_initial_removal(&board1, 3, 3, true)) {
    printf("Failed to setup test position\n");
    return;
  }
  if (!execute_initial_removal(&board1, 3, 2, false)) {
    printf("Failed to setup test position\n");
    return;
  }
  
  print_board(&board1);
  
  // Expected: At depth 1, Black should have moves
  printf("Depth 1 (Black to move): ");
  uint64_t nodes1 = perft_nodes(&board1, false, 1);
  printf("%llu nodes\n", (unsigned long long)nodes1);
  
  if (nodes1 == 0) {
    printf("ERROR: No moves from initial position!\n");
    printf("Checking move generation directly...\n");
    
    MoveSequence moves[MAX_SEQUENCES];
    int num_moves = generate_all_moves(&board1, false, moves);
    printf("generate_all_moves returned: %d moves\n", num_moves);
    
    if (num_moves > 0) {
      printf("Sample move: ");
      print_move_sequence(&moves[0]);
      printf("\n");
    }
  }
  
  // Test 2: Simple forced jump position
  printf("\nTest 2: Forced jump position\n");
  Board board2;
  init_board(&board2);
  
  // Create: White at B2, Black at B3, empty at B4
  // Clear the board
  board2.white = 0;
  board2.black = 0;
  board2.occupied = 0;
  board2.empty = VALID_MASK;
  
  // Setup forced jump
  set_white(&board2, 1, 1);  // B2
  set_black(&board2, 1, 2);  // B3
  // B4 is already empty
  
  print_board(&board2);
  
  printf("Depth 1 (White to move): ");
  uint64_t nodes2 = perft_nodes(&board2, true, 1);
  printf("%llu nodes (expected: 1 - forced jump B2->B4)\n", 
         (unsigned long long)nodes2);
  
  // Test 3: Empty board (no moves for anyone)
  printf("\nTest 3: Empty board\n");
  Board board3;
  init_board(&board3);
  board3.white = 0;
  board3.black = 0;
  board3.occupied = 0;
  board3.empty = VALID_MASK;
  
  printf("Depth 1 (White to move): ");
  uint64_t nodes3 = perft_nodes(&board3, true, 1);
  printf("%llu nodes (expected: 1 - terminal position)\n", 
         (unsigned long long)nodes3);
}

// Quick diagnostic
void perft_diagnostic(void) {
  printf("\n=== PERFT DIAGNOSTIC ===\n");
  
  Board board;
  init_board(&board);
  
  // Check initial board
  printf("Initial board (full):\n");
  print_board(&board);
  
  // Check if moves can be generated
  MoveSequence moves[MAX_SEQUENCES];
  
  printf("\n1. Testing move generation for Black (before opening):\n");
  int num_moves_black = generate_all_moves(&board, false, moves);
  printf("   generate_all_moves(board, false, ...) = %d\n", num_moves_black);
  
  printf("\n2. Testing move generation for White (before opening):\n");
  int num_moves_white = generate_all_moves(&board, true, moves);
  printf("   generate_all_moves(board, true, ...) = %d\n", num_moves_white);
  
  // Do opening removals
  printf("\n3. Performing opening removals...\n");
  if (!execute_initial_removal(&board, 3, 3, true)) {
    printf("   Failed to remove black stone at D4\n");
  }
  
  if (!execute_initial_removal(&board, 3, 2, false)) {
    printf("   Failed to remove white stone at D3\n");
  }
  
  printf("Board after opening:\n");
  print_board(&board);
  
  printf("\n4. Testing move generation for Black (after opening):\n");
  num_moves_black = generate_all_moves(&board, false, moves);
  printf("   generate_all_moves(board, false, ...) = %d\n", num_moves_black);
  
  if (num_moves_black > 0) {
    printf("   Sample move: ");
    print_move_sequence(&moves[0]);
    printf("\n");
    
    // Try to execute it
    Board test_board = board;
    if (execute_sequence(&test_board, &moves[0], false)) {
      printf("   Move execution SUCCESS\n");
    } else {
      printf("   Move execution FAILED\n");
    }
  }
  
  printf("\n5. Running perft depth 1:\n");
  uint64_t nodes = perft_nodes(&board, false, 1);
  printf("   perft_nodes(board, false, 1) = %llu\n", (unsigned long long)nodes);
}

void perft_menu(void) {
  while (1) {
    printf("\n-- Perft / Performance Tests --\n");
    printf("(1) Perft nodes (single depth)\n");
    printf("(2) Perft divide (show per-move counts)\n");
    printf("(3) Negamax benchmark\n");
    printf("(4) Run preset suite\n");
    printf("(5) Diagnostic tests\n");
    printf("(6) Test suite (known positions)\n");
    printf("(0) Back\n");
    printf("> ");

    int cmd = -1;
    if (scanf("%d", &cmd) != 1) break;

    if (cmd == 0) break;

    Board board;
    init_board(&board);
    
    // Do standard opening for most tests
    execute_initial_removal(&board, 3, 3, true);
    execute_initial_removal(&board, 3, 2, false);

    switch (cmd) {
      case 1: {
        int depth = 1;
        printf("Depth: "); scanf("%d", &depth);
        uint64_t nodes = perft_nodes(&board, false, depth);
        printf("Perft nodes (depth %d): %llu\n", depth, (unsigned long long)nodes);
        break;
      }
      case 2: {
        int depth = 2;
        printf("Depth: "); scanf("%d", &depth);
        perft_divide(&board, false, depth);
        break;
      }
      case 3: {
        int depth = 3;
        int iterations = 10;
        printf("Depth: "); scanf("%d", &depth);
        printf("Iterations: "); scanf("%d", &iterations);
        uint64_t nodes = 0;
        double t = perft_benchmark_negamax(&board, false, depth, iterations, &nodes);
        printf("Negamax benchmark: iterations=%d, depth=%d, total_time=%.4fs, avg=%.6fs, nodes=%llu\n",
               iterations, depth, t, t / iterations, (unsigned long long)nodes);
        break;
      }
      case 4: {
        printf("Running preset suite (depths 1..4) as Black:\n");
        for (int d = 1; d <= 4; d++) {
          clock_t s = clock();
          uint64_t n = perft_nodes(&board, false, d);
          clock_t e = clock();
          double secs = (double)(e - s) / CLOCKS_PER_SEC;
          printf(" depth %d: %llu nodes, time=%.4fs\n", d, (unsigned long long)n, secs);
        }
        break;
      }
      case 5:
        perft_diagnostic();
        break;
      case 6:
        perft_test_suite();
        break;
      default:
        printf("Unknown command\n");
    }
  }
}

// Benchmark wrapper: runs negamax repeatedly and counts nodes
double perft_benchmark_negamax(const Board *board, bool is_white_turn, 
                              int depth, int iterations, uint64_t *nodes_out) {
  if (!board || depth <= 0 || iterations <= 0) {
    if (nodes_out) *nodes_out = 0;
    return 0.0;
  }
  
  uint64_t total_nodes = 0;
  
  // Modified negamax that counts nodes
  int negamax_with_counter(Board *b, int d, bool is_white, 
                          int alpha, int beta, uint64_t *counter) {
    if (counter) (*counter)++;
    
    if (d == 0) {
      return eval_position(b, is_white);
    }
    
    MoveSequence moves[MAX_SEQUENCES];
    int num_moves = generate_all_moves(b, is_white, moves);
    
    if (num_moves == 0) {
      return -10000 + d;
    }
    
    int best_score = INT_MIN;
    
    for (int i = 0; i < num_moves; i++) {
      Board bcopy = *b;
      if (!execute_sequence(&bcopy, &moves[i], is_white)) {
        continue;
      }
      
      int score = -negamax_with_counter(&bcopy, d - 1, !is_white, 
                                      -beta, -alpha, counter);
      
      if (score > best_score) best_score = score;
      if (score > alpha) alpha = score;
      if (alpha >= beta) break;
    }
    
    return best_score;
  }
  
  // Count nodes for one search
  Board bcopy = *board;
  uint64_t nodes_per_search = 0;
  (void)negamax_with_counter(&bcopy, depth, is_white_turn, 
                            INT_MIN, INT_MAX, &nodes_per_search);
  
  if (nodes_out) {
    *nodes_out = nodes_per_search;
  }
  
  // Time multiple iterations
  clock_t start = clock();
  for (int i = 0; i < iterations; i++) {
    Board bcopy2 = *board;
    negamax(&bcopy2, depth, is_white_turn, INT_MIN, INT_MAX, NULL);
  }
  clock_t end = clock();
  
  double elapsed_seconds = (double)(end - start) / (double)CLOCKS_PER_SEC;
  return elapsed_seconds;
}
