#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10
#define OBSTACLE_COUNT 10

void printLabyrinth(char labyrinth[SIZE][SIZE]);
int checkWin(int playerRow, int playerCol, int treasureRow, int treasureCol);

int main(void)
{
    char labyrinth[SIZE][SIZE] = {{' '}};
    int playerRow, playerCol;
    int treasureRow, treasureCol;
    int row, col;
    int placedObstacles = 0;
    char input;

    srand((unsigned int)time(NULL));

    playerRow = rand() % SIZE;
    playerCol = rand() % SIZE;

    do
    {
        treasureRow = rand() % SIZE;
        treasureCol = rand() % SIZE;
    }
    while (treasureRow == playerRow && treasureCol == playerCol);

    labyrinth[playerRow][playerCol] = 'P';
    labyrinth[treasureRow][treasureCol] = 'T';

    while (placedObstacles < OBSTACLE_COUNT)
    {
        row = rand() % SIZE;
        col = rand() % SIZE;

        if (labyrinth[row][col] == ' ')
        {
            labyrinth[row][col] = 'O';
            placedObstacles++;
        }
    }

    printf("=== Labyrinth-Spiel ===\n");
    printf("Steuerung: W = hoch, A = links, S = runter, D = rechts\n\n");
    printLabyrinth(labyrinth);

    while (scanf(" %c", &input) == 1)
    {
        int newRow = playerRow;
        int newCol = playerCol;

        if (input == 'W')
        {
            newRow--;
        }
        else if (input == 'A')
        {
            newCol--;
        }
        else if (input == 'S')
        {
            newRow++;
        }
        else if (input == 'D')
        {
            newCol++;
        }
        else
        {
            printf("Ungueltige Eingabe.\n");
            continue;
        }

        if (newRow < 0 || newRow >= SIZE || newCol < 0 || newCol >= SIZE ||
            labyrinth[newRow][newCol] == 'O')
        {
            continue;
        }

        labyrinth[playerRow][playerCol] = ' ';
        playerRow = newRow;
        playerCol = newCol;
        labyrinth[playerRow][playerCol] = 'P';
        printLabyrinth(labyrinth);

        if (checkWin(playerRow, playerCol, treasureRow, treasureCol))
        {
            printf("Schatz gefunden!\n");
            break;
        }
    }

    return 0;
}

int checkWin(int playerRow, int playerCol, int treasureRow, int treasureCol)
{
    return playerRow == treasureRow && playerCol == treasureCol;
}

void printLabyrinth(char labyrinth[SIZE][SIZE])
{
    int row, col;

    for (row = 0; row < SIZE; row++)
    {
        for (col = 0; col < SIZE; col++)
        {
            printf(" %c ", labyrinth[row][col]);
        }
        printf("\n");
    }
}
