#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#define GRID_DIM 10
#define TOTAL_SHIPS_NUMBER 5
#define HORIZONTAL 0
#define VERTICAL 1
#define RESULT_CORRECT 0
#define RESULT_INCORRECT 1
#define SHOOT_MODE 0
#define DEPLOY_MODE 1
#define MAX_SHIP_LENGTH 5
#define MULTIPLAYER 1
#define SINGLEPLAYER 2


/*AI self-referential structure */

typedef struct shoot
{
   int row;
   int column;
   struct shoot *next;
} shoot_t;

///////////////////////////////

struct coordinates
{
    int row[MAX_SHIP_LENGTH];
    int column[MAX_SHIP_LENGTH];
    int orientation;
};

typedef struct Ship
{
    char name[32];
    int length;
    int lifes;
    /*Ship position */
    struct coordinates ShipCoordinates;
    struct Ship *next;
} Ship;

/* Grid function*/
int ***createGrid();
void printGrid(int***,int);
void shotIndicatedField(int***grid, int selectPlayer,Ship** EnemyShips);
int checkPositionPermitted(int ***grid,int number, int orientation, Ship** MyShips);
void printBoards(int*** grid, int selectGrid);
int selectGameMode(int ***grid, Ship** p1, Ship**p2);

/* Ships function*/
Ship **createShips(void);
void deployShips(int*** myShipsGrid, Ship** MyShips);
void addShipsManually(int*** myShipsGrid, Ship **MyShips);
void randShipsPosition(int ***grid, Ship** MyShips);
void setShipAtPosition(int*** myShipsGrid,int ship_number, int orientation, Ship** MyShips);
void shipHit(int wanted_row,int wanted_column,Ship** EnemyShips);


void printStats(Ship** MyStats);

/* helper fnc*/
struct coordinates getInput(int selectMode);
void selectedFieldOutOfRange(void);
void nextPlayer(void);
int end_game(Ship** P1, Ship** P2);
void error_recovery_invalid_field(void);
void makeGapBetweenGrids(int countCycles);



//////////////////////
void playAgainstAI(int***new_grid, Ship** EnemyShips);
struct shoot chooseFieldToShoot(int numberHurtLifes);
void shotFieldAI(int***grid, Ship **EnemyShips, struct shoot nextFieldToShoot);
void getLastHitFields(int number, int *last_hits_rows, int *last_hits_columns);
void checkPossibleHorizontal(int*last_hits_rows, int*last_hits_columns,int*flag_left_position, int*flag_right_position, int numberHurtLifes);
struct shoot selectPositionsHorizontally(int*last_hits_rows, int*last_hits_columns,int flag_up_position, int flag_down_position, int numberHurtLifes);
void checkPossibleVeritical(int*last_hits_rows, int*last_hits_columns,int*flag_up_position, int*flag_down_position, int numberHurtLifes);
struct shoot selectPositionsVertically(int*last_hits_rows, int*last_hits_columns,int flag_up_position, int flag_down_position, int numberHurtLifes);

void append(struct shoot**, int new_row, int new_column);

void updateInvalidFields(int number,Ship** EnemyShips);

int checkDamagedShip(Ship** EnemyShips);
struct shoot randPositionToShoot(void);
int checkShotList(int rowToCheck, int columnToCheck);
void bubbleSort(int *arr, int arraySize);
////////////////////////
#endif // GRID_H_INCLUDED
