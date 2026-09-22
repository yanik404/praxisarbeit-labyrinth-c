#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10

void printLabyrinth(char labyrinth[SIZE][SIZE]);

int main(void)
{
    char labyrinth[SIZE][SIZE] = {{' '}};
    int playerRow, playerCol;
    int treasureRow, treasureCol;

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

    printf("=== Labyrinth-Spiel ===\n");
    printLabyrinth(labyrinth);
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
