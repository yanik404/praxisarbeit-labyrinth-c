#include <stdio.h>

#define SIZE 10

void printLabyrinth(char labyrinth[SIZE][SIZE]);

int main(void)
{
    char labyrinth[SIZE][SIZE] = {{' '}};

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
