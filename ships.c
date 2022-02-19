#ifndef _STDIO_H_
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <unistd.h>
#include <ctype.h>
#include "grid.h"
#endif // _STDIO_H_

jmp_buf env, read_input_again;

void selectedFieldOutOfRange(void)
{
   // printf("\nDetected invalid field, enter coordinates again!\n");
    longjmp(env, 1);
}

void error_recovery_invalid_field(void)
{
    longjmp(read_input_again, 1);
}

static int player = 0;

Ship **createShips(void)
{
    Ship **Ships;
    Ships = malloc(sizeof(Ship*)*TOTAL_SHIPS_NUMBER);

    const char *shipsName[] =
    {
        "Carrier",
        "Battleship",
        "Cruiser",
        "Submarine",
        "Destroyer"
    };

    int ships_size[] = {5,4,3,3,2};

    for(int i = 0; i < TOTAL_SHIPS_NUMBER; i++)
    {
        Ships[i] = malloc(sizeof(Ship));

        strcpy(Ships[i] -> name, shipsName[i]);
        Ships[i] -> length =ships_size[i];
        Ships[i] -> lifes = ships_size[i];

        for(int j; j<MAX_SHIP_LENGTH; j++)
        {
            Ships[i] -> ShipCoordinates.row[j] = 0;
            Ships[i] -> ShipCoordinates.column[j] = 0;
        }
    }

    return Ships;
}


/*  No ships may be placed on another ship and must not contact each other*/
void deployShips(int*** myShipsGrid, Ship** MyShips)
{


    printf("                    PLAYER %d\n", player+1);
    int selected_action=0;

    while(!selected_action)
    {
        printf("\nPress '1' to select ships position manually: \nPress '2' to select ships position automatically: \n");
        scanf("%d", &selected_action);

        switch(selected_action)
        {
        case 1:
            printf("\n          Player %d deploy ships\n\n  Use format: (e.g. A5h)\n  Where 'h' is horizontal and 'v' is vertical \n",(player+1));
            addShipsManually(myShipsGrid,MyShips);
            break;
        case 2:
            randShipsPosition(myShipsGrid,MyShips);
            nextPlayer();
            break;
        default:
            printf("\n...Invalid Choice...\n");
            selected_action = 0;
        }
    }

    printf("\nPlayer %d, allocated all ships!\n\n",(player+1));
    player++;
}

void addShipsManually(int*** myShipsGrid, Ship **MyShips)
{
    for(int cnt=0; cnt < TOTAL_SHIPS_NUMBER; cnt++ )
    {
        int check_pos_result = 0, orientation_int=0;

        /*Error Handler when the field specified by the user is invalid  */
        setjmp(read_input_again);
        /* Print information which ship deploy */
        printGrid(myShipsGrid,player);
        printf("'%s' size %d\n",MyShips[cnt] -> name,MyShips[cnt] ->length);
        printf("Enter position: \n ");
        /* Read data from stdin*/
        struct coordinates input_result = getInput(DEPLOY_MODE);

        /* Write data to ship struct */
        MyShips[cnt]-> ShipCoordinates.row[0] = input_result.row[0];
        MyShips[cnt]-> ShipCoordinates.column[0] = input_result.column[0];

        orientation_int = input_result.orientation;

        if(orientation_int == HORIZONTAL)
        {
            check_pos_result = checkPositionPermitted(myShipsGrid,cnt,HORIZONTAL,MyShips);

            if(check_pos_result==RESULT_CORRECT)
            {
                setShipAtPosition(myShipsGrid,cnt,orientation_int,MyShips);
            }
            else
            {
                error_recovery_invalid_field();
            }
        }
        else if(orientation_int == VERTICAL)
        {
            check_pos_result = checkPositionPermitted(myShipsGrid,cnt,VERTICAL,MyShips);

            if(check_pos_result==RESULT_CORRECT)
            {
                setShipAtPosition(myShipsGrid,cnt,orientation_int,MyShips);
            }
            else
            {
                error_recovery_invalid_field();
            }
        }


    }//end for
}

/* Read coordinates from stdin*/
struct coordinates getInput(int selectMode)
{
     /*Error handler position out off range: repeat read input */
    setjmp(env);

    struct coordinates result;
    int row_int=0, column_int =0;
    char  column_c='\0',orientation_c='\0';
    char *readInput;
    size_t bufsize = 5;
    readInput = (char *)malloc(bufsize * sizeof(char));

    /* Read data from stdin */
    getline(&readInput,&bufsize,stdin);
    fflush(stdin);

    /* check 1st is column */
    sscanf(readInput,"%c%d%c",&column_c,&row_int,&orientation_c);
    column_c = toupper(column_c);
    if( (column_c>='A' && column_c<='J') && (row_int >=1 && row_int <= 10))
    {
    }
    else
    {
        /* check 1st is row */
        sscanf(readInput,"%d%c%c",&row_int,&column_c,&orientation_c);
        column_c = toupper(column_c);
        if( (column_c>='A' && column_c<='J') && (row_int >=1 && row_int <= 10))
        {

        }
        else
        {
            /*If entered string is incorrect Repeat read input */
            selectedFieldOutOfRange();
        }
    }

    /*Convert column sign to int value and decrease row number -1*/
    for (char i='A'; i<column_c; i++)
    {
        column_int++;
    }

    row_int -= 1;

    /* Saved value after conversion*/
    result.row[0] = row_int;
    result.column[0] = column_int;

    if(selectMode==DEPLOY_MODE)
    {
    /*Convert read char to uppercase letters.*/
    orientation_c = toupper(orientation_c);
    /* Check that the orientation has been correctly entered*/
    if(orientation_c== 'H')
        {
            result.orientation = HORIZONTAL;
        }
        else if(orientation_c== 'V')
        {
            result.orientation = VERTICAL;
        }
        else
        {
            selectedFieldOutOfRange();
        }
    }

    free(readInput);
    return result;
}

int checkPositionPermitted(int ***grid,int number, int orientation, Ship** MyShips)
{
    int result=0;
    int row_to_verify =  MyShips[number]-> ShipCoordinates.row[0];
    int column_to_verify = MyShips[number]-> ShipCoordinates.column[0];
    int ship_size =  MyShips[number]-> length;

    if(( orientation == HORIZONTAL) && ((column_to_verify+ship_size)<=GRID_DIM)) // if no collsion with wall check rest of
    {
        result = RESULT_CORRECT;

        /* If ship's last field isnt in last column */
        if( (column_to_verify != 0) && ((column_to_verify + ship_size) < GRID_DIM) && (  (grid[player][row_to_verify][column_to_verify+ship_size] == 1 ) || (grid[player][row_to_verify][column_to_verify-1] ==1 ) ))
        {
            result = RESULT_INCORRECT;
        }
        else if( (column_to_verify == 0) && (grid[player][row_to_verify][column_to_verify+ship_size] == 1 ) )
        {
            result = RESULT_INCORRECT;
        }
        else if (( (column_to_verify + ship_size) == GRID_DIM) && (grid[player][row_to_verify][column_to_verify-1] == 1 ))
        {
            result = RESULT_INCORRECT;
        }

        // look for other ships
        for(int i=0; i<ship_size; i++)
        {
            /* check selected row is not collision */
            if(grid[player][row_to_verify][column_to_verify] == 1)
            {
                result = RESULT_INCORRECT;
                break;
            }
            /* if 1st row check only row below is not collision */
            if(row_to_verify == 0)
            {
                if(grid[player][row_to_verify+1][column_to_verify] == 1)
                {
                    result = RESULT_INCORRECT;
                    break;
                }
            }
            /* if last row check only row above */
            else if(row_to_verify == 9)
            {
                if(grid[player][row_to_verify-1][column_to_verify] == 1)
                {
                    result = RESULT_INCORRECT;
                    break;
                }
            }
            /* else check row below and above is not collision */
            else
            {
                if( (grid[player][row_to_verify+1][column_to_verify] == 1) || (grid[player][row_to_verify-1][column_to_verify] == 1) )
                {
                    result = RESULT_INCORRECT;
                    break;
                }
            }

            // if(row !=0 && row

            column_to_verify++;
        } // end for
    }
    else if(( orientation == VERTICAL) && ((row_to_verify+ship_size)<=GRID_DIM)) //detect wall
    {
        result=RESULT_CORRECT;

        /* If ship's last field isnt 1ST AND LAST  */
        if( (row_to_verify != 0) && ((row_to_verify + ship_size) < GRID_DIM) && (  (grid[player][row_to_verify-1][column_to_verify] == 1 ) || (grid[player][row_to_verify+ship_size][column_to_verify] ==1 ) ))
        {
            result = RESULT_INCORRECT;
        }
        else if( (row_to_verify == 0) && (grid[player][row_to_verify+ship_size][column_to_verify] == 1 ) ) // zaczyna sie w 1 wierszu sprawdz tylko dolny koniec + 1
        {
            result = RESULT_INCORRECT;
        }
        else if (( (row_to_verify + ship_size) == GRID_DIM) && (grid[player][row_to_verify-1][column_to_verify] == 1 )) // JESLI koczy sie w ostatnim wierszu to sprawdz tylko poczatek
        {
            result = RESULT_INCORRECT;
        }

        // look for other ships
        for(int i=0; i<ship_size; i++)
        {
            if(grid[player][row_to_verify][column_to_verify] == 1) // detected collision
            {
                result = RESULT_INCORRECT;
                break;
            }

            /* if 1st row check only row below is not collision */
            if(column_to_verify == 0)
            {
                if(grid[player][row_to_verify][column_to_verify+1] == 1)
                {
                    result = RESULT_INCORRECT;
                    break;
                }
            }
            /* if last row check only row above */
            else if(column_to_verify == 9)
            {
                if(grid[player][row_to_verify][column_to_verify-1] == 1)
                {
                    result = RESULT_INCORRECT;
                    break;
                }
            }
            /* else check row below and above is not collision */
            else
            {
                if( (grid[player][row_to_verify][column_to_verify+1] == 1) || (grid[player][row_to_verify][column_to_verify-1] == 1) )
                {
                    result = RESULT_INCORRECT;
                    break;
                }
            }

            row_to_verify++;
        } // end for
    }
    else
    {
        result=RESULT_INCORRECT;
    }

    // check flag
//    if(result==RESULT_CORRECT)
//    {
//        printf("Field is correct \n");
//    }
//    else
//    {
//        printf("Field is incorrect!\n");
//    }

    return result;
}


void setShipAtPosition(int*** myShipsGrid,int ship_number, int orientation, Ship** MyShips) // PLAYER
{
    int row = MyShips[ship_number]-> ShipCoordinates.row[0];
    int column = MyShips[ship_number]-> ShipCoordinates.column[0];

    int ship_size = MyShips[ship_number] -> length;

    if(orientation == HORIZONTAL)
    {
        for(int k=0; k <ship_size; k++)
        {
            /* Set value '1' on board */
            myShipsGrid[player][row][column] = 1;
            /* save ships coordinates */
            MyShips[ship_number]-> ShipCoordinates.row[k] = row;
            MyShips[ship_number]-> ShipCoordinates.column[k] = column;
            column++;
        }
    }
    else if(orientation == VERTICAL)
    {
        for(int j=0; j <ship_size; j++)
        {
            /* Set value '1' on board */
            myShipsGrid[player][row][column] = 1;
            /* save ships coordinates */
            MyShips[ship_number]-> ShipCoordinates.row[j] = row;
            MyShips[ship_number]-> ShipCoordinates.column[j] = column;
            row++;
        }
    }
}


void randShipsPosition(int ***grid, Ship** MyShips)
{
    int drown_orientation=0, drown_row =0,drown_column=0;// When orientation is: 0=horizontal and 1=vertical
    int result=0, cnt=0;
    time_t seed;
    srand((unsigned) time(&seed));

    while (cnt<TOTAL_SHIPS_NUMBER)
    {
        drown_row = rand() % (GRID_DIM); // 0-9
        drown_column = rand() % (GRID_DIM);
        drown_orientation=rand() % (2);

        MyShips[cnt]-> ShipCoordinates.row[0] = drown_row;
        MyShips[cnt]-> ShipCoordinates.column[0] = drown_column;

        result = checkPositionPermitted(grid,cnt,drown_orientation,MyShips);

        if(result==RESULT_CORRECT)
        {
            setShipAtPosition(grid,cnt,drown_orientation,MyShips);
            cnt++;
        }
    }

}

void shipHit(int wanted_row, int wanted_column, Ship** EnemyShips)
{
    int flag_exit=0, temp1=0,temp2=0;

    for(int number=0; number <TOTAL_SHIPS_NUMBER; number++)
    {
        if(flag_exit==1)
        {
            break;
        }

        for(int cnt=0; cnt< MAX_SHIP_LENGTH; cnt++)
        {
            temp1 = EnemyShips[number] -> ShipCoordinates.row[cnt];
            temp2 = EnemyShips[number] -> ShipCoordinates.column[cnt];

            /* When you find a ship on that field, subtract one life and exit the loop */
            if( (temp1 == wanted_row) && (temp2 == wanted_column) )
            {
                EnemyShips[number] -> lifes -=1;
                flag_exit=1;

                 /* updateInvalid() fields after destroy ship */
                if( (EnemyShips[number] -> lifes) == 0)
                {
                  updateInvalidFields(number,EnemyShips);
                }
                break;
            }
        }
    }

}


