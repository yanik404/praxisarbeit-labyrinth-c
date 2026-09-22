#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10
#define OBSTACLE_COUNT 10

void printLabyrinth(char labyrinth[SIZE][SIZE]);

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
        if (input == 'W')
        {
            playerRow--;
        }
        else if (input == 'A')
        {
            playerCol--;
        }
        else if (input == 'S')
        {
            playerRow++;
        }
        else if (input == 'D')
        {
            playerCol++;
        }
        else
        {
            break;
        }

        labyrinth[playerRow][playerCol] = 'P';
        printLabyrinth(labyrinth);
    }

    return 0;
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
