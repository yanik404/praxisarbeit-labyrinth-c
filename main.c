#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define SIZE 10
#define OBSTACLE_COUNT 10

void initLabyrinth(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol,
                   int *treasureRow, int *treasureCol);
void printLabyrinth(char labyrinth[SIZE][SIZE]);
int movePlayer(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol, char input);
int checkWin(int playerRow, int playerCol, int treasureRow, int treasureCol);

int main(void)
{
    char labyrinth[SIZE][SIZE];
    int playerRow, playerCol;
    int treasureRow, treasureCol;
    char input;
    int running = 1;

    srand((unsigned int)time(NULL));

    initLabyrinth(labyrinth, &playerRow, &playerCol, &treasureRow, &treasureCol);

    printf("=== Labyrinth-Spiel ===\n");
    printf("Steuerung: W = hoch, A = links, S = runter, D = rechts, Q = beenden\n\n");
    printLabyrinth(labyrinth);

    while (running)
    {
        printf("\nEingabe: ");

        if (scanf(" %c", &input) != 1)
        {
            printf("Eingabefehler.\n");
            break;
        }

        input = (char)toupper((unsigned char)input);

        if (input == 'Q')
        {
            printf("Spiel beendet.\n");
            running = 0;
        }
        else if (movePlayer(labyrinth, &playerRow, &playerCol, input))
        {
            printLabyrinth(labyrinth);

            if (checkWin(playerRow, playerCol, treasureRow, treasureCol))
            {
                printf("\nGewonnen! Du hast den Schatz gefunden.\n");
                running = 0;
            }
        }
        else
        {
            printf("Ungueltige Bewegung.\n");
        }
    }

    return 0;
}

void initLabyrinth(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol,
                   int *treasureRow, int *treasureCol)
{
    int row, col;
    int placedObstacles = 0;

    for (row = 0; row < SIZE; row++)
    {
        for (col = 0; col < SIZE; col++)
        {
            labyrinth[row][col] = ' ';
        }
    }

    *playerRow = rand() % SIZE;
    *playerCol = rand() % SIZE;

    do
    {
        *treasureRow = rand() % SIZE;
        *treasureCol = rand() % SIZE;
    }
    while (*treasureRow == *playerRow && *treasureCol == *playerCol);

    labyrinth[*playerRow][*playerCol] = 'P';
    labyrinth[*treasureRow][*treasureCol] = 'T';

    while (placedObstacles < OBSTACLE_COUNT)
    {
        int onSafePath;

        row = rand() % SIZE;
        col = rand() % SIZE;

        /* Eine einfache L-foermige Verbindung bleibt frei, damit der Schatz erreichbar ist. */
        onSafePath =
            (col == *playerCol &&
             row >= ((*playerRow < *treasureRow) ? *playerRow : *treasureRow) &&
             row <= ((*playerRow > *treasureRow) ? *playerRow : *treasureRow)) ||
            (row == *treasureRow &&
             col >= ((*playerCol < *treasureCol) ? *playerCol : *treasureCol) &&
             col <= ((*playerCol > *treasureCol) ? *playerCol : *treasureCol));

        if (labyrinth[row][col] == ' ' && !onSafePath)
        {
            labyrinth[row][col] = 'O';
            placedObstacles++;
        }
    }
}

void printLabyrinth(char labyrinth[SIZE][SIZE])
{
    int row, col;

    printf("+");
    for (col = 0; col < SIZE; col++)
    {
        printf("---+");
    }
    printf("\n");

    for (row = 0; row < SIZE; row++)
    {
        printf("|");
        for (col = 0; col < SIZE; col++)
        {
            printf(" %c |", labyrinth[row][col]);
        }
        printf("\n+");

        for (col = 0; col < SIZE; col++)
        {
            printf("---+");
        }
        printf("\n");
    }
}

int movePlayer(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol, char input)
{
    int newRow = *playerRow;
    int newCol = *playerCol;

    switch (input)
    {
        case 'W':
            newRow--;
            break;
        case 'A':
            newCol--;
            break;
        case 'S':
            newRow++;
            break;
        case 'D':
            newCol++;
            break;
        default:
            return 0;
    }

    if (newRow < 0 || newRow >= SIZE || newCol < 0 || newCol >= SIZE)
    {
        return 0;
    }

    if (labyrinth[newRow][newCol] == 'O')
    {
        return 0;
    }

    labyrinth[*playerRow][*playerCol] = ' ';
    *playerRow = newRow;
    *playerCol = newCol;
    labyrinth[*playerRow][*playerCol] = 'P';


    return 1;
}

int checkWin(int playerRow, int playerCol, int treasureRow, int treasureCol)
{
    return playerRow == treasureRow && playerCol == treasureCol;
}
