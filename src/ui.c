#include "ui.h"

void main_menu(void) {
  printf("-- \033[1mK O N A N E\033[0m --\n");

  int n = -1;

  while (n < 1 || n > 3) {
    printf("(1) Play PvP\n(2) Play PvAI\n(3) Watch AIvAI\n");
    printf("> ");
    scanf("%d", &n);
  }

  switch(n) {
    case 1:
      run_game(GAME_MODE_HUMAN_VS_HUMAN);
      break;
    case 2:
      run_game(GAME_MODE_HUMAN_VS_AI);
      break;  
    case 3:
      run_game(GAME_MODE_AI_VS_AI);
      break;
  }
}