#include "graphic_view.h"

#include <raylib.h>

#define TILE_SIZE 64
#define BOARD_SIZE (SIZE * TILE_SIZE)
#define SIDEBAR_WIDTH 330
#define WINDOW_WIDTH (BOARD_SIZE + SIDEBAR_WIDTH)
#define WINDOW_HEIGHT (BOARD_SIZE + 90)

void initLabyrinth(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol,
                   int *treasureRow, int *treasureCol);
int movePlayer(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol, char input);
int checkWin(int playerRow, int playerCol, int treasureRow, int treasureCol);

static const Color dungeonBackground = {12, 20, 28, 255};
static const Color floorColor = {38, 49, 58, 255};
static const Color wallColor = {100, 105, 109, 255};
static const Color wallShadow = {42, 47, 52, 255};
static const Color goldColor = {239, 177, 70, 255};

static void drawLabyrinth(char labyrinth[SIZE][SIZE]);
static void drawPlayer(int row, int col);
static void drawTreasure(int row, int col);
static void drawSidebar(int moves);
static void drawWinScreen(int moves);
static void drawTorch(int x, int y);
static void drawKey(int x, int y, const char *label);
static int handleMovement(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol);

void startGraphicGame(void)
{
    char labyrinth[SIZE][SIZE];
    int playerRow, playerCol;
    int treasureRow, treasureCol;
    int moves = 0;
    int won = 0;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Labyrinth-Spiel");
    SetTargetFPS(60);
    initLabyrinth(labyrinth, &playerRow, &playerCol, &treasureRow, &treasureCol);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_Q))
        {
            break;
        }

        if (won)
        {
            if (IsKeyPressed(KEY_R))
            {
                initLabyrinth(labyrinth, &playerRow, &playerCol, &treasureRow, &treasureCol);
                moves = 0;
                won = 0;
            }
        }
        else if (handleMovement(labyrinth, &playerRow, &playerCol))
        {
            moves++;
            won = checkWin(playerRow, playerCol, treasureRow, treasureCol);
        }

        BeginDrawing();
        ClearBackground(dungeonBackground);
        drawLabyrinth(labyrinth);
        drawPlayer(playerRow, playerCol);
        drawTreasure(treasureRow, treasureCol);
        drawSidebar(moves);

        DrawRectangle(0, BOARD_SIZE, WINDOW_WIDTH, 90, (Color){7, 13, 19, 255});
        DrawRectangleLines(10, BOARD_SIZE + 10, WINDOW_WIDTH - 20, 70, goldColor);
        DrawText("Ein dunkles Labyrinth. Irgendwo hier wartet ein Schatz ...", 28, BOARD_SIZE + 35, 22, LIGHTGRAY);

        if (won)
        {
            drawWinScreen(moves);
        }
        EndDrawing();
    }

    CloseWindow();
}

static int handleMovement(char labyrinth[SIZE][SIZE], int *playerRow, int *playerCol)
{
    char input = '\0';

    if (IsKeyPressed(KEY_W)) input = 'W';
    if (IsKeyPressed(KEY_A)) input = 'A';
    if (IsKeyPressed(KEY_S)) input = 'S';
    if (IsKeyPressed(KEY_D)) input = 'D';

    if (input == '\0')
    {
        return 0;
    }

    return movePlayer(labyrinth, playerRow, playerCol, input);
}

static void drawLabyrinth(char labyrinth[SIZE][SIZE])
{
    int row, col;

    for (row = 0; row < SIZE; row++)
    {
        for (col = 0; col < SIZE; col++)
        {
            int x = col * TILE_SIZE;
            int y = row * TILE_SIZE;

            DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, floorColor);
            DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, (Color){61, 72, 80, 255});
            DrawCircle(x + 16, y + 18, 2, (Color){75, 91, 83, 255});
            DrawLine(x + 38, y + 14, x + 47, y + 22, (Color){27, 36, 42, 255});
            DrawLine(x + 47, y + 22, x + 43, y + 32, (Color){27, 36, 42, 255});

            if (labyrinth[row][col] == 'O')
            {
                DrawRectangle(x + 3, y + 3, TILE_SIZE - 6, TILE_SIZE - 6, wallShadow);
                DrawRectangle(x + 5, y + 5, TILE_SIZE - 10, TILE_SIZE - 10, wallColor);
                DrawLine(x + 8, y + 26, x + TILE_SIZE - 8, y + 26, wallShadow);
                DrawLine(x + 23, y + 7, x + 23, y + 26, wallShadow);
                DrawLine(x + 43, y + 27, x + 43, y + TILE_SIZE - 7, wallShadow);
            }
        }
    }

    drawTorch(28, 100);
    drawTorch(BOARD_SIZE - 38, 245);
    drawTorch(BOARD_SIZE / 2, BOARD_SIZE - 100);
    DrawRectangleLines(0, 0, BOARD_SIZE, BOARD_SIZE, (Color){168, 129, 79, 255});
}

static void drawPlayer(int row, int col)
{
    int x = col * TILE_SIZE + TILE_SIZE / 2;
    int y = row * TILE_SIZE + TILE_SIZE / 2;

    DrawEllipse(x, y + 31, 17, 5, (Color){10, 15, 20, 130});
    DrawCircle(x, y - 12, 12, (Color){241, 191, 143, 255});
    DrawRectangle(x - 14, y, 28, 22, (Color){43, 112, 161, 255});
    DrawRectangle(x - 16, y + 21, 12, 12, (Color){39, 50, 68, 255});
    DrawRectangle(x + 4, y + 21, 12, 12, (Color){39, 50, 68, 255});
    DrawCircle(x - 4, y - 13, 2, BLACK);
    DrawCircle(x + 5, y - 13, 2, BLACK);
}

static void drawTreasure(int row, int col)
{
    int x = col * TILE_SIZE + 12;
    int y = row * TILE_SIZE + 18;

    DrawCircle(x + 20, y + 25, 30, (Color){235, 161, 45, 35});
    DrawRectangle(x, y + 16, 40, 25, (Color){113, 61, 29, 255});
    DrawRectangle(x, y + 9, 40, 13, (Color){159, 94, 35, 255});
    DrawRectangleLines(x, y + 9, 40, 32, goldColor);
    DrawRectangle(x + 17, y + 20, 7, 10, goldColor);
    DrawCircle(x + 20, y + 25, 2, YELLOW);
}

static void drawSidebar(int moves)
{
    int x = BOARD_SIZE;

    DrawRectangle(x, 0, SIDEBAR_WIDTH, BOARD_SIZE, (Color){8, 15, 22, 255});
    DrawLine(x, 0, x, BOARD_SIZE, goldColor);
    DrawText("Labyrinth-", x + 52, 48, 38, goldColor);
    DrawText("Spiel", x + 95, 92, 38, goldColor);
    DrawLine(x + 35, 145, x + SIDEBAR_WIDTH - 35, 145, goldColor);

    DrawRectangleLines(x + 35, 180, SIDEBAR_WIDTH - 70, 120, goldColor);
    DrawText("Zuege:", x + 55, 202, 28, RAYWHITE);
    DrawText(TextFormat("%d", moves), x + 128, 240, 45, goldColor);

    DrawText("Steuerung:", x + 55, 350, 27, RAYWHITE);
    drawKey(x + 55, 392, "W");
    drawKey(x + 105, 392, "A");
    drawKey(x + 155, 392, "S");
    drawKey(x + 205, 392, "D");
    DrawText("bewegen", x + 86, 440, 23, LIGHTGRAY);
    drawKey(x + 55, 480, "Q");
    DrawText("beenden", x + 110, 490, 24, LIGHTGRAY);
    DrawText("Finde die Truhe!", x + 55, 575, 22, GRAY);
    DrawCircleLines(x + SIDEBAR_WIDTH / 2, 625, 42, goldColor);
    DrawLine(x + SIDEBAR_WIDTH / 2, 575, x + SIDEBAR_WIDTH / 2, 675, goldColor);
    DrawLine(x + SIDEBAR_WIDTH / 2 - 50, 625, x + SIDEBAR_WIDTH / 2 + 50, 625, goldColor);
}

static void drawWinScreen(int moves)
{
    DrawRectangle(90, 220, BOARD_SIZE - 180, 180, (Color){4, 10, 15, 235});
    DrawRectangleLines(90, 220, BOARD_SIZE - 180, 180, goldColor);
    DrawCircle(BOARD_SIZE / 2, 246, 18, goldColor);
    DrawText("+", BOARD_SIZE / 2 - 7, 233, 26, (Color){113, 61, 29, 255});
    DrawText("Du hast den Schatz gefunden!", 140, 255, 30, goldColor);
    DrawText(TextFormat("Zuege: %d", moves), 255, 305, 28, RAYWHITE);
    DrawText("R = Neues Spiel   Q = Beenden", 160, 350, 22, LIGHTGRAY);
}

static void drawTorch(int x, int y)
{
    DrawCircle(x, y, 26, (Color){235, 123, 33, 35});
    DrawRectangle(x - 3, y, 6, 25, (Color){101, 58, 31, 255});
    DrawTriangle((Vector2){(float)x, (float)(y - 25)}, (Vector2){(float)(x - 9), (float)(y - 2)},
                 (Vector2){(float)(x + 9), (float)(y - 2)}, ORANGE);
    DrawCircle(x, y - 10, 7, YELLOW);
}

static void drawKey(int x, int y, const char *label)
{
    DrawRectangleRounded((Rectangle){(float)x, (float)y, 38, 38}, 0.2f, 4, (Color){40, 49, 62, 255});
    DrawRectangleRoundedLines((Rectangle){(float)x, (float)y, 38, 38}, 0.2f, 4, LIGHTGRAY);
    DrawText(label, x + 12, y + 8, 21, RAYWHITE);
}
