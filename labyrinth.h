#ifndef LABYRINTH_H
#define LABYRINTH_H

#define SIZE 10

void initLabyrinth(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol,
                   int *treasureRow, int *treasureCol);
int movePlayer(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol, char input);
/* The graphical dungeon uses the same movement rules on a larger field. */
int movePlayerOnField(char *field, int rows, int cols,
                      int *playerRow, int *playerCol, char input);
int checkWin(int playerRow, int playerCol, int treasureRow, int treasureCol);

#endif
