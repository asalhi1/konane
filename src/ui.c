/* Minimal text-based UI for selecting gameplay and benchmarks. */
#include "ui.h"
#include "perft.h"

// Display main menu
void main_menu(void) {
  printf("-- \033[1mK O N A N E\033[0m --\n");

  int n = -1;

  while (n < 1 || n > 4) {
    printf("(1) Play PvP\n(2) Play PvAI\n(3) Watch AIvAI\n(4) Perft / Benchmarks\n");
    printf("> ");
    scanf("%d", &n);
  }

  switch(n) {
    case 1:
      /* Player vs Player */
      run_game(GAME_MODE_HUMAN_VS_HUMAN);
      break;
    case 2:
      /* Player vs AI */
      run_game(GAME_MODE_HUMAN_VS_AI);
      break;  
    case 3:
      /* AI vs AI spectator mode */
      run_game(GAME_MODE_AI_VS_AI);
      break;
    case 4:
      /* Perft and benchmarking utilities */
      perft_menu();
      break;
  }
} 