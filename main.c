#ifndef _STDIO_H_
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <unistd.h>
#include <windows.h>
#include <ctype.h>
#include "grid.h"
#endif // _STDIO_H_

int main()
{
    int player1 = 0;
    int player2 = 1;

    int*** new_grid = createGrid();

    Ship** Player_1 = createShips();
    Ship** Player_2 = createShips();

    int const gameMode = selectGameMode(new_grid,Player_1,Player_2);

    int game_score = end_game(Player_1,Player_2);

    while(game_score)
    {
        /* Player 1 action */
        printBoards(new_grid,player1); // Player 1
        printStats(Player_2);
        shotIndicatedField(new_grid, player1, Player_2);
        nextPlayer();

        /* Player 2 action depended from selected mode*/
        if(gameMode == MULTIPLAYER)
        {
            printBoards(new_grid,player2); // Player 2
            printStats(Player_1);
            shotIndicatedField(new_grid, player2 ,Player_1);
            nextPlayer();
        }
        else
        {
            playAgainstAI(new_grid, Player_1);
        }

        /* Check game is not finished */
        game_score = end_game(Player_1,Player_2);
    }

    printf("End game. Congratulations to all players!\n");

    return 0;
}


