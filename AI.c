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

typedef shoot_t* ListShootPtr;

ListShootPtr head_all_shoots = NULL;
ListShootPtr head_hit_shoots  = NULL;


/* main AI algorithm */
void playAgainstAI(int***new_grid, Ship** EnemyShips)
{
    int damagedShip = checkDamagedShip(EnemyShips);

    struct shoot nextFieldToShoot;

    if(damagedShip)
    {
        nextFieldToShoot = chooseFieldToShoot(damagedShip);
    }
    else
    {
        nextFieldToShoot = randPositionToShoot();
    }

    shotFieldAI(new_grid,EnemyShips, nextFieldToShoot);

}

struct shoot chooseFieldToShoot(int numberHurtLifes)
{
    /*results */
    struct shoot choosenField;

    int flag_left_position=0, flag_right_position=0, flag_up_position=0, flag_down_position=0;
    /* get last hit fields*/
    int *last_hits_rows = (int*)malloc(numberHurtLifes*sizeof(int));
    int *last_hits_columns = (int*)malloc(numberHurtLifes*sizeof(int));

    /* get last hit fields*/
    getLastHitFields(numberHurtLifes,last_hits_rows,last_hits_columns);

    /* Bubble sort for 2 and more ship's hit, that [0] index was the lowest number*/
    bubbleSort(last_hits_rows,numberHurtLifes);
    bubbleSort(last_hits_columns,numberHurtLifes);

    /* if w zaleznosci czy jest 1 trafienie czy wiecej */
    if(numberHurtLifes==1)
    {
        checkPossibleHorizontal(last_hits_rows,last_hits_columns,&flag_right_position,&flag_left_position,numberHurtLifes);
        checkPossibleVeritical(last_hits_rows,last_hits_columns,&flag_up_position,&flag_down_position,numberHurtLifes);

        if( flag_left_position && flag_right_position ) // if horizontal is impossible: go vertical
        {
            choosenField = selectPositionsVertically(last_hits_rows,last_hits_columns,flag_up_position,flag_down_position,numberHurtLifes);
        }
        else if(flag_up_position && flag_down_position ) // if vertical is impossible: go horizontal
        {
            choosenField = selectPositionsHorizontally(last_hits_rows,last_hits_columns,flag_right_position,flag_left_position,numberHurtLifes);

        }
        else // if both are possible draw
        {
            time_t seed;
            srand((unsigned) time(&seed));

            int randOrientation = rand() % (2); // 0 or 1

            if(randOrientation)
            {
                choosenField = selectPositionsHorizontally(last_hits_rows,last_hits_columns,flag_right_position,flag_left_position,numberHurtLifes);
            }
            else
            {
                choosenField = selectPositionsVertically(last_hits_rows,last_hits_columns,flag_up_position,flag_down_position,numberHurtLifes);
            }
        }
    }
    else
    {
        if(last_hits_rows[0] == last_hits_rows[1] ) // if ship located HORIZONTAL
        {
            checkPossibleHorizontal(last_hits_rows,last_hits_columns,&flag_right_position,&flag_left_position,numberHurtLifes);
            choosenField = selectPositionsHorizontally(last_hits_rows,last_hits_columns,flag_right_position,flag_left_position,numberHurtLifes); // zwrocic wartosc !
        }
        else if(last_hits_columns[0] == last_hits_columns[1]) // if ship located VERTICAL
        {
            checkPossibleVeritical(last_hits_rows,last_hits_columns,&flag_up_position,&flag_down_position,numberHurtLifes);
            choosenField = selectPositionsVertically(last_hits_rows,last_hits_columns,flag_up_position,flag_down_position,numberHurtLifes);
        }
    }

    free(last_hits_rows);
    free(last_hits_columns);

    return choosenField;

}

void append(struct shoot** head, int new_row, int new_column)
{
    ListShootPtr new_shoot =(struct shoot*) malloc(sizeof(shoot_t));
    new_shoot -> row = new_row;
    new_shoot -> column = new_column;

    new_shoot -> next = *head;

    *head = new_shoot;
}

void shotFieldAI(int***grid, Ship **EnemyShips, struct shoot nextFieldToShoot) // change to int return value np. 1 przy trafieniu , 1 przy pudle
{
    int row=0, col=0;
    int opponentGrid=0, selectPlayer = 1;

    row = nextFieldToShoot.row;
    col = nextFieldToShoot.column;

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
        append(&head_hit_shoots,row,col); // add hit shot list
    }

    append(&head_all_shoots,row,col); // add all shot list
}


void getLastHitFields(int number, int *last_hits_rows, int *last_hits_columns)
{
    ListShootPtr current  = head_hit_shoots;

    int cnt=0;

    while(cnt<number)
    {
        last_hits_rows[cnt] = current->row;
        last_hits_columns[cnt] = current->column;

        cnt++;
        current = current->next; // set pointer to next NODE
    }
}

void checkPossibleHorizontal(int*last_hits_rows, int*last_hits_columns,int*flag_right_position, int*flag_left_position, int numberHurtLifes)
{
    /* Check position is not occur at all shots list */
    int row = last_hits_rows[0];
    int left_column = (last_hits_columns[0]) - 1;
    int right_column = (last_hits_columns[numberHurtLifes-1]) + 1;
    int resultLeft= checkShotList(row,left_column);
    int resultRight=checkShotList(row,right_column);

    /* check next fields on the left and right arent out off grid */
    if( (left_column < 0) || resultLeft )
    {
        *flag_left_position = RESULT_INCORRECT;
    }
    else
    {
        *flag_left_position  = RESULT_CORRECT;
    }
    if ( (right_column >= GRID_DIM) || resultRight )
    {
        *flag_right_position = RESULT_INCORRECT;
    }
    else
    {
        *flag_right_position = RESULT_CORRECT;
    }

}

struct shoot selectPositionsHorizontally(int*last_hits_rows, int*last_hits_columns,int flag_right_position, int flag_left_position, int numberHurtLifes)
{
    struct shoot results;
    int choosenRow=0,choosenColumn=0;

    /* check flags both side to shoot are possbile*/
    if( (!flag_left_position) && (!flag_right_position) )
    {
        int randDirection =rand() % (2); // 0 - 1

        if(randDirection)
        {
            choosenColumn = (last_hits_columns[0]- 1) ;
            choosenRow = last_hits_rows[0];
        }
        else
        {
            choosenColumn = (last_hits_columns[numberHurtLifes-1]+1);
            choosenRow = last_hits_rows[0];
        }

    }
    else if(!flag_left_position)
    {
        choosenColumn = (last_hits_columns[0]- 1) ;
        choosenRow = last_hits_rows[0];
    }
    else if (!flag_right_position)
    {
        choosenColumn = (last_hits_columns[numberHurtLifes-1]+1);
        choosenRow = last_hits_rows[0];
    }

    results.row = choosenRow;
    results.column = choosenColumn;

    return results;

}


void checkPossibleVeritical(int*last_hits_rows, int*last_hits_columns,int*flag_up_position, int*flag_down_position, int numberHurtLifes)
{
    /* Check position is not occur at all shots list */
    int column = last_hits_columns[0];
    int up_row = last_hits_rows[0] - 1;
    int down_row = last_hits_rows[numberHurtLifes-1] + 1 ;
    int resultUp= checkShotList(up_row,column);
    int resultDown=checkShotList(down_row,column);

    /* check next fields on the up and down arent out off grid */
    if( (up_row < 0) || resultUp )
    {
        *flag_up_position = RESULT_INCORRECT;
    }
    else
    {
        *flag_up_position  = RESULT_CORRECT;
    }
    if ( (down_row >= GRID_DIM) || resultDown )
    {
        *flag_down_position = RESULT_INCORRECT;
    }
    else
    {
        *flag_down_position = RESULT_CORRECT;
    }
}

struct shoot selectPositionsVertically(int *last_hits_rows, int *last_hits_columns,int flag_up_position, int flag_down_position, int numberHurtLifes)
{
    struct shoot results;
    int choosenRow=0,choosenColumn=0;
    /* check flags both side to shoot are possbile*/
    if( (!flag_up_position) && (!flag_down_position) )
    {
        time_t seed;
        srand((unsigned) time(&seed));
        int randDirection =rand() % (2); // 0 - 1

        if(randDirection)
        {
            choosenColumn =  last_hits_columns[0];
            choosenRow = (last_hits_rows[0]-1);
        }
        else
        {
            choosenColumn = last_hits_columns[0];
            choosenRow = (last_hits_rows[numberHurtLifes-1]+1);
        }

    }
    else if(!flag_up_position)
    {
        choosenColumn = last_hits_columns[0];
        choosenRow = (last_hits_rows[0] -1);
    }
    else if (!flag_down_position)
    {
        choosenColumn = last_hits_columns[0];
        choosenRow = (last_hits_rows[numberHurtLifes-1]+1);
    }

    results.row = choosenRow;
    results.column = choosenColumn;

    return results;
}



int checkDamagedShip(Ship** EnemyShips)
{
    int ships_size[] = {5,4,3,3,2};
    int ship_lifes=0, numberOfHitsOnShip  = 0;

    for(int i=0; i<TOTAL_SHIPS_NUMBER; i++)
    {
        ship_lifes = EnemyShips[i] -> lifes;

        /* Ship must be hit, but not sunk  */
        if( (ship_lifes < ships_size[i]) && (ship_lifes != 0) )
        {
            numberOfHitsOnShip = ships_size[i] - ship_lifes;
            break;
        }
    }

    return numberOfHitsOnShip;
}



struct shoot randPositionToShoot(void)
{
    struct shoot RowColumn;
    int drown_row =0,drown_column=0, result=0;
    time_t seed;
    srand((unsigned) time(&seed));

    do
    {
        drown_row = rand() % (GRID_DIM); // 0-9
        drown_column = rand() % (GRID_DIM);

        result = checkShotList(drown_row,drown_column);
    }
    while(result);

    RowColumn.row = drown_row;
    RowColumn.column = drown_column;

    return RowColumn;
}

int checkShotList(int rowToCheck, int columnToCheck)
{
    ListShootPtr current = head_all_shoots;
    int current_row=0, current_column=0, result = 0;

    while(current != NULL)
    {
        current_row = current-> row;
        current_column = current-> column;

        if( (current_row == rowToCheck) && (current_column == columnToCheck)) //search by name
        {
            result = 1; // set 1 if that cooradinates has already been
            break;
        }
        else
        {
            current = current->next; // set pointer to next NODE
        }
    }

    return result;
}


void bubbleSort(int *array, int arraySize)
{
    for (int step = 0; step < arraySize - 1; ++step)
    {
        for (int i = 0; i < arraySize - step - 1; ++i)
        {
            if (array[i] > array[i + 1])
            {
                int temp = array[i];
                array[i] = array[i + 1];
                array[i + 1] = temp;
            }
        }
    }
}


void updateInvalidFields(int number,Ship** EnemyShips)
{
    int row=0, col=0, positionFound=0;

    int shipLength = EnemyShips[number] -> length;

    for(int i=0; i<shipLength; i++)
    {
        row = EnemyShips[number] -> ShipCoordinates.row[i];
        col = EnemyShips[number] -> ShipCoordinates.column[i];

            positionFound = checkShotList(row-1,col); // row above
            if(!positionFound && ((row-1) >= 0) )
            {
                append(&head_all_shoots,row-1,col);
            }

            positionFound = checkShotList(row+1,col); // row below
            if(!positionFound && ((row+1) <= GRID_DIM) )
            {
                append(&head_all_shoots,row+1,col);
            }

            positionFound = checkShotList(row,col-1); // col left
            if(!positionFound && ((col-1) >=0) )
            {
                append(&head_all_shoots,row,col-1);
            }

            positionFound = checkShotList(row,col+1); // col right
            if(!positionFound && ((col+1) <= GRID_DIM) )
            {
                append(&head_all_shoots,row,col+1);
            }
    }
}
