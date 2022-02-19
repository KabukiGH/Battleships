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

int*** createGrid(void)
{
    int ***fields = NULL;

    int array[10][10] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    fields = (int***)malloc(4*sizeof(int**));

    for(int grid=0; grid<4; grid++) // exit 4 grids; my ships player 1, my shots player 1, my ships player 2, my shots player 2,
    {
        fields[grid] = (int**)malloc(GRID_DIM*sizeof(int*));

        for(int i = 0; i<GRID_DIM; i++)
        {
            fields[grid][i] = (int*)malloc(GRID_DIM*sizeof(int));

            for(int j=0; j<GRID_DIM; j++)
            {
                fields[grid][i][j] = array[i][j];
            }
        }
    }

    return fields;
}

void printGrid(int*** grid, int selectGrid)
{

    if(selectGrid==0 || selectGrid==1) // myShip board: player1,player2
    {
        printf("\n                         Player %d my board\n\n",selectGrid+1);
    }
    else
    {
        printf("\n                         Opponent board \n\n");
    }

    /* Print letters and top frame*/
    printf("   ");
    for(char top_row='A'; top_row<'K'; top_row++)
    {
        printf("  %c   ", top_row);
    }
    printf("\n   ------------------------------------------------------------\n");
    for(int i = 0; i<GRID_DIM; i++)
    {
        /* Print Left frame || Align the single and double-digit numbers */
        if(i<GRID_DIM-1)
        {
            printf(" %d| ",(i+1));
        }
        else
        {
            printf("%d| ",(i+1));
        }

        /* Print value inside board*/
        for(int j=0; j<GRID_DIM; j++)
        {
            /*  Display inside a board (change int value to properrly chars*/
            if(grid[selectGrid][i][j]==0) //clear field
            {
                printf("   ");
            }
            else if(grid[selectGrid][i][j]==1) // with ship
            {
                printf(" O ");
            }
            else if(grid[selectGrid][i][j]==2)// missed shot
            {
                printf(" - ");
            }
            else if(grid[selectGrid][i][j]==3) // ship hitted
            {
                printf(" X ");
            }

            printf(" | ");

        } // end j loop
        /* Print bottom frame*/
        printf("\n  |-----------------------------------------------------------|\n");
    } // end i loop

}

int selectGameMode(int ***new_grid, Ship** Player_1, Ship** Player_2)
{
    printf("\n\n\n\n\n\n\n\n                         Before starting the game, zoom in on your screen and then press enter. Have fun :)\n\n");
    int selectedGameMode=0;
    char ch;
    while(ch!='\n'){
        ch=getchar();}

    while(!selectedGameMode)
    {
    printf("                        Battleships\nSelect game mode:\n1) Two Players\n2) Play against computer\n3) Exit\n");
    scanf("%d", &selectedGameMode);

        switch(selectedGameMode)
        {
        case MULTIPLAYER:
            deployShips(new_grid,Player_1); // player 1
            nextPlayer();
            deployShips(new_grid,Player_2); // player 2
            nextPlayer();
            break;
        case SINGLEPLAYER:
            deployShips(new_grid,Player_1); // player 1
            nextPlayer();
            randShipsPosition(new_grid,Player_2);
            break;
        case 3:
            exit(0);
        default:
            printf("\n...Invalid Choice...\n");
            selectedGameMode=0;
        }
    }

    return selectedGameMode;
}

void shotIndicatedField(int***grid, int selectPlayer,Ship **EnemyShips) // change to int return value np. 1 przy trafieniu , 1 przy pudle
{
    int opponentGrid = 0, row=0, col=0;
    // int coordinates[2] = {0};
    struct coordinates result;

    if(selectPlayer==0)
    {
        opponentGrid=1;
    }
    else if(selectPlayer==1)
    {
        opponentGrid=0;
    }

    printf("Enter position to SHOT: \n ");
    result = getInput(SHOOT_MODE);
    row = result.row[0];
    col = result.column[0];

    if(grid[opponentGrid][row][col] == 0) //empty
    {
        grid[opponentGrid][row][col] = 2; // usaw siatke ze statkami rywala
        grid[selectPlayer+2][row][col] = 2; // change value at my shot board
    }
    else if(grid[opponentGrid][row][col] == 1) //ship
    {
        grid[opponentGrid][row][col] = 3; // HIT, add function -1
        grid[selectPlayer+2][row][col] = 3; // change value at my shot board

        shipHit(row, col, EnemyShips);
    }

}

void printStats(Ship **MyStats)
{
    int ship_lifes=0;
    printf("\n");
    for(int i=0; i<TOTAL_SHIPS_NUMBER; i++)
    {
        ship_lifes = MyStats[i] -> lifes;

        if(ship_lifes>0)
        {
            printf("%s %d lifes\n",MyStats[i] -> name,ship_lifes);
        }
    }
    printf("\n");
}

void nextPlayer(void)
{
    char ch='\0';

    for(int i=0; i<10; i++)
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    while(ch!='\n')
    {
        ch=getchar();
    }

    system("@cls||clear");
}

int end_game(Ship** P1, Ship** P2)
{
    int remaining_lifes_P1 =0, remaining_lifes_P2 =0;

    for(int i=0; i< TOTAL_SHIPS_NUMBER; i++)
    {
        remaining_lifes_P1 += P1[i] -> lifes;
        remaining_lifes_P2 += P2[i] -> lifes;
    }

    if( (remaining_lifes_P1==0) || (remaining_lifes_P2==0) )
    {
        return 0; // end game
    }
    else
    {
        return 1; // continue game
    }
}

void printBoards(int*** grid, int selectGrid)
{
    printf("\n                         Opponent board                                                                         Player %d my board \n\n", selectGrid+1);
    selectGrid += 2;
    for(int x=0; x<2; x++)
    {

        /* Print letters and top frame*/
        printf("   ");
        for(char top_row='A'; top_row<'K'; top_row++)
        {
            printf("  %c   ", top_row);
        }
       makeGapBetweenGrids(x);
    }

    for(int x=0; x<2; x++)
    {

        printf("   ------------------------------------------------------------");
        makeGapBetweenGrids(x);
    }


    for(int i = 0; i<GRID_DIM; i++)
    {
        for(int x=0, current = selectGrid; x<2; x++,current-=2)
        {
            /* Print Left frame || Align the single and double-digit numbers */
            if(i<GRID_DIM-1)
            {
                printf(" %d| ",(i+1));
            }
            else
            {
                printf("%d| ",(i+1));
            }

            /* Print value inside board*/
            for(int j=0; j<GRID_DIM; j++)
            {
                /*  Display inside a board (change int value to properrly chars*/
                if(grid[current][i][j]==0) //clear field
                {
                    printf("   ");
                }
                else if(grid[current][i][j]==1) // with ship
                {
                    printf(" O ");
                }
                else if(grid[current][i][j]==2)// missed shot
                {
                    printf(" - ");
                }
                else if(grid[current][i][j]==3) // ship hitted
                {
                    printf(" X ");
                }

                printf(" | ");

            } // end j loop

          makeGapBetweenGrids(x);
        } // end x loop

        /* Print bottom frame*/
        for(int x =0; x<2; x++)
        {
            if(x==1)
            {
                printf(" ");
            }
            printf("  |-----------------------------------------------------------|");

           makeGapBetweenGrids(x);
        }
    }// end i loop
}

void makeGapBetweenGrids(int countCycles)
{
    printf("                      ");
    if(countCycles==1)
    {
    printf("\n");
    }
}

