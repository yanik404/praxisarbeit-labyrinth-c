#include "graphic_view.h"
#include "labyrinth.h"
#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Draw at a small, fixed resolution and scale with nearest-neighbour pixels. */
#define VIEW_WIDTH 720
#define VIEW_HEIGHT 510
#define COLS 29
#define ROWS 23
#define TILE 18
#define BOARD_X 18
#define BOARD_Y 20
#define STEP_TIME 0.13f
#define TORCH_COUNT 4

typedef struct {
    char field[ROWS][COLS];
    int playerRow, playerCol, treasureRow, treasureCol;
    int moves, facing, won;
    float fromRow, fromCol, step, time, winTime, blockedTime;
    int torchRow[TORCH_COUNT], torchCol[TORCH_COUNT];
} GraphicGame;

static Texture2D sprites;
static Font titleFont, textFont;
static const Color ink = {6, 14, 21, 255};
static const Color bronze = {96, 67, 40, 255};
static const Color gold = {237, 177, 77, 255};
static const Color parchment = {212, 209, 203, 255};

static float clamp01(float x) { return fminf(1.0f, fmaxf(0.0f, x)); }

static unsigned int detailNoise(int x, int y)
{
    unsigned int n = (unsigned int)x * 374761393u + (unsigned int)y * 668265263u;
    n = (n ^ (n >> 13)) * 1274126177u;
    return n ^ (n >> 16);
}

static const char *assetPath(const char *name)
{
    static char path[1024];
    const char *prefixes[] = {"assets/", "../assets/"};
    for (int i = 0; i < 2; i++)
    {
        snprintf(path, sizeof(path), "%s%s", prefixes[i], name);
        if (FileExists(path)) return path;
        snprintf(path, sizeof(path), "%s%s%s", GetApplicationDirectory(), prefixes[i], name);
        if (FileExists(path)) return path;
    }
    return NULL;
}

static int loadGraphics(void)
{
    const char *path = assetPath("dungeon_sprites.png");
    if (!path) return 0;
    sprites = LoadTexture(path);
    if (!sprites.id) return 0;
    SetTextureFilter(sprites, TEXTURE_FILTER_POINT);

    int codepoints[224];
    for (int i = 0; i < 224; i++) codepoints[i] = i + 32;
    path = assetPath("PirataOne-Regular.ttf");
    titleFont = path ? LoadFontEx(path, 28, codepoints, 224) : GetFontDefault();
    path = assetPath("VT323-Regular.ttf");
    textFont = path ? LoadFontEx(path, 20, codepoints, 224) : GetFontDefault();
    SetTextureFilter(titleFont.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(textFont.texture, TEXTURE_FILTER_POINT);
    return 1;
}

static void unloadGraphics(void)
{
    if (titleFont.texture.id != GetFontDefault().texture.id) UnloadFont(titleFont);
    if (textFont.texture.id != GetFontDefault().texture.id) UnloadFont(textFont);
    UnloadTexture(sprites);
}

static void sprite(Rectangle source, float x, float y, float w, float h, Color tint)
{
    DrawTexturePro(sprites, source, (Rectangle){roundf(x), roundf(y), w, h},
                   (Vector2){0, 0}, 0, tint);
}

static void label(Font font, const char *text, int x, int y, float size, Color color)
{
    DrawTextEx(font, text, (Vector2){(float)x + 1, (float)y + 1}, size, 0, BLACK);
    DrawTextEx(font, text, (Vector2){(float)x, (float)y}, size, 0, color);
}

static void centered(Font font, const char *text, int x, int y, int width, float size, Color color)
{
    int offset = (int)((width - MeasureTextEx(font, text, size, 0).x) / 2);
    label(font, text, x + offset, y, size, color);
}

static void frame(int x, int y, int w, int h)
{
    DrawRectangle(x + 3, y, w - 6, h, ink);
    DrawRectangle(x, y + 3, w, h - 6, ink);
    DrawLine(x + 4, y, x + w - 5, y, bronze);
    DrawLine(x + 4, y + h - 1, x + w - 5, y + h - 1, bronze);
    DrawLine(x, y + 4, x, y + h - 5, bronze);
    DrawLine(x + w - 1, y + 4, x + w - 1, y + h - 5, bronze);
    for (int side = 0; side < 2; side++)
    {
        int cx = side ? x + w - 4 : x + 2;
        DrawRectangle(cx, y + 2, 2, 2, bronze);
        DrawRectangle(cx, y + h - 4, 2, 2, bronze);
    }
}

static void wallLine(GraphicGame *g, int x1, int y1, int x2, int y2)
{
    for (int row = y1; row <= y2; row++)
        for (int col = x1; col <= x2; col++) g->field[row][col] = 'O';
}

static void referenceDungeon(GraphicGame *g)
{
    /* Connected wall segments follow the composition of the reference. */
    const int walls[][4] = {
        {0,0,28,0}, {0,22,28,22}, {0,0,0,22}, {28,0,28,22},
        {12,0,12,4}, {22,0,22,5}, {22,5,24,5},
        {3,3,8,3}, {5,3,5,7}, {8,3,8,10}, {3,7,5,7}, {3,7,3,10}, {0,10,3,10},
        {8,7,16,7}, {16,3,16,7}, {16,3,19,3}, {19,3,19,8},
        {19,8,23,8}, {23,8,23,12}, {17,12,23,12}, {26,8,28,8},
        {6,10,11,10}, {6,10,6,15}, {6,15,9,15}, {9,13,9,20},
        {14,10,17,10}, {14,10,14,18}, {17,10,17,16}, {12,13,14,13},
        {20,16,25,16}, {25,12,25,16}, {20,16,20,18}, {12,18,20,18}, {12,18,12,22},
        {3,13,5,13}, {3,13,3,19}, {3,19,7,19}, {7,19,7,22},
        {23,19,26,19}, {23,19,23,22}
    };
    memset(g->field, ' ', sizeof(g->field));
    for (unsigned int i = 0; i < sizeof(walls) / sizeof(walls[0]); i++)
        wallLine(g, walls[i][0], walls[i][1], walls[i][2], walls[i][3]);
    g->field[12][6] = ' '; /* Passage from the starting alcove into the main maze. */
    g->field[9][23] = ' ';
    g->playerRow = 10; g->playerCol = 5;
    g->treasureRow = 2; g->treasureCol = 26;
}

static void randomDungeon(GraphicGame *g)
{
    /* A simple depth-first walk carves a connected, solvable new maze. */
    int stack[ROWS * COLS][2], count = 1;
    const int dr[] = {-2, 0, 2, 0}, dc[] = {0, 2, 0, -2};
    memset(g->field, 'O', sizeof(g->field));
    stack[0][0] = 1; stack[0][1] = 1;
    g->field[1][1] = ' ';
    while (count)
    {
        int row = stack[count - 1][0], col = stack[count - 1][1];
        int choices[4], found = 0;
        for (int d = 0; d < 4; d++)
        {
            int nr = row + dr[d], nc = col + dc[d];
            if (nr > 0 && nr < ROWS - 1 && nc > 0 && nc < COLS - 1 && g->field[nr][nc] == 'O')
                choices[found++] = d;
        }
        if (!found) { count--; continue; }
        int d = choices[rand() % found];
        int nr = row + dr[d], nc = col + dc[d];
        g->field[row + dr[d] / 2][col + dc[d] / 2] = ' ';
        g->field[nr][nc] = ' ';
        stack[count][0] = nr; stack[count++][1] = nc;
    }
    g->playerRow = ROWS - 2; g->playerCol = 1;
    g->treasureRow = 1; g->treasureCol = COLS - 2;
}

static void startRound(GraphicGame *g, int firstRound)
{
    memset(g, 0, sizeof(*g));
    if (firstRound) referenceDungeon(g); else randomDungeon(g);
    g->field[g->playerRow][g->playerCol] = 'P';
    g->field[g->treasureRow][g->treasureCol] = 'T';
    g->fromRow = (float)g->playerRow; g->fromCol = (float)g->playerCol;
    g->step = 1; g->facing = 1;
    const int desired[TORCH_COUNT][2] = {{2,3}, {5,14}, {10,27}, {20,14}};
    for (int i = 0; i < TORCH_COUNT; i++)
    {
        int best = ROWS * COLS;
        for (int r = 1; r < ROWS - 1; r++)
            for (int c = 1; c < COLS - 1; c++)
            {
                int distance = abs(r - desired[i][0]) + abs(c - desired[i][1]);
                if (g->field[r][c] == ' ' && distance < best)
                {
                    best = distance; g->torchRow[i] = r; g->torchCol[i] = c;
                }
            }
    }
}

static void updateGame(GraphicGame *g, float dt, char input)
{
    g->time += dt;
    g->blockedTime = fmaxf(0, g->blockedTime - dt);
    if (g->step < 1)
    {
        g->step = clamp01(g->step + dt / STEP_TIME);
        return;
    }
    if (g->won)
    {
        g->winTime += dt;
        if (input == 'R') startRound(g, 0);
        return;
    }
    if (!input) return;
    g->fromRow = (float)g->playerRow; g->fromCol = (float)g->playerCol;
    if (input == 'A') g->facing = -1;
    if (input == 'D') g->facing = 1;
    if (movePlayerOnField(&g->field[0][0], ROWS, COLS, &g->playerRow, &g->playerCol, input))
    {
        g->moves++;
        g->step = 0;
        g->won = checkWin(g->playerRow, g->playerCol, g->treasureRow, g->treasureCol);
    }
    else g->blockedTime = 0.3f;
}

static char readMovement(void)
{
    if (IsKeyPressed(KEY_R)) return 'R';
    if (IsKeyDown(KEY_W)) return 'W';
    if (IsKeyDown(KEY_A)) return 'A';
    if (IsKeyDown(KEY_S)) return 'S';
    if (IsKeyDown(KEY_D)) return 'D';
    return 0;
}

static void drawStoneDetails(int x, int y, int row, int col)
{
    unsigned int n = detailNoise(col, row);
    for (int i = 0; i < 12; i++)
    {
        n = n * 1664525u + 1013904223u;
        int dx = 2 + (int)(n % 14), dy = 2 + (int)((n >> 5) % 12);
        DrawRectangle(x + dx, y + dy, 1 + (int)((n >> 9) % 3), 1,
                      (i % 2) ? (Color){187,175,162,70} : (Color){29,31,35,90});
    }
}

static void drawFloor(int x, int y, int row, int col)
{
    /* One slab per cell; avoid shrinking the entire 3x3 floor sheet into each cell. */
    int sx = 557 + (col % 3) * 114, sy = 176 + (row % 3) * 114;
    sprite((Rectangle){(float)sx, (float)sy, 110, 110}, x, y, TILE, TILE, (Color){181,190,205,255});
    DrawLine(x, y + TILE - 1, x + TILE, y + TILE - 1, (Color){20,25,29,255});
    DrawLine(x + TILE - 1, y, x + TILE - 1, y + TILE, (Color){20,25,29,255});
    unsigned int n = detailNoise(row, col);
    if (n % 11 == 0)
    {
        for (int i = 0; i < 6; i++)
            DrawRectangle(x + (int)((n >> i) % 13), y + (int)((n >> (i+6)) % 13), 2, 1,
                          (Color){55,64,37,180});
    }
}

static void drawWall(const GraphicGame *g, int row, int col)
{
    int x = BOARD_X + col * TILE, y = BOARD_Y + row * TILE;
    int below = row + 1 == ROWS || g->field[row + 1][col] != 'O';
    if (below)
    {
        DrawRectangle(x + 3, y + 15, TILE + 3, 10, (Color){0,0,0,90});
        sprite((Rectangle){90 + (col % 3)*112.0f, 284, 105, 116}, x, y + 10, TILE, 12, (Color){151,155,161,255});
        DrawLine(x, y + 21, x + TILE, y + 21, (Color){11,15,20,255});
        if (detailNoise(row, col) % 5 == 0)
            DrawRectangle(x + 4, y + 15, 7, 3, (Color){45,57,32,210});
    }
    sprite((Rectangle){90 + (col % 3)*112.0f, 177, 105, 80}, x, y - 4, TILE, TILE, WHITE);
    drawStoneDetails(x, y - 4, row, col);
    DrawLine(x, y - 4, x + TILE, y - 4, (Color){183,177,168,255});
    DrawLine(x, y - 3, x, y + 13, (Color){50,47,47,255});
    DrawLine(x, y + 13, x + TILE, y + 13, (Color){43,42,43,255});
}

static void drawDecoration(int x, int y, int skull)
{
    DrawEllipse(x, y + 4, 9, 3, (Color){0,0,0,90});
    DrawRectangle(x - 5, y - 2, 9, 7, (Color){104,105,101,255});
    DrawRectangle(x - 3, y - 4, 6, 2, (Color){140,138,126,255});
    DrawRectangle(x + 4, y + 1, 4, 3, (Color){65,70,72,255});
    if (skull)
    {
        DrawRectangle(x - 4, y - 1, 2, 3, (Color){20,23,27,255});
        DrawRectangle(x + 1, y - 1, 2, 3, (Color){20,23,27,255});
        DrawRectangle(x - 2, y + 5, 4, 2, (Color){141,137,119,255});
    }
}

static void drawLabyrinth(const GraphicGame *g)
{
    DrawRectangle(BOARD_X - 5, BOARD_Y - 9, COLS * TILE + 10, ROWS * TILE + 21, (Color){35,33,32,255});
    DrawRectangle(BOARD_X - 2, BOARD_Y - 6, COLS * TILE + 4, ROWS * TILE + 15, BLACK);
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            drawFloor(BOARD_X + c * TILE, BOARD_Y + r * TILE, r, c);
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (g->field[r][c] == 'O') drawWall(g, r, c);
    for (int r = 2; r < ROWS - 2; r++)
        for (int c = 2; c < COLS - 2; c++)
            if (g->field[r][c] == ' ' && detailNoise(r,c) % 61 == 0)
                drawDecoration(BOARD_X + c*TILE + 9, BOARD_Y + r*TILE + 10, detailNoise(c,r)%2);
}

static void drawTorch(int x, int y, float time, int index)
{
    float pulse = 0.84f + 0.10f * sinf(time * 9 + index) + 0.06f * sinf(time * 17);
    DrawCircleGradient(x, y - 8, 42 * pulse, (Color){251,151,47,52}, (Color){251,110,20,0});
    DrawRectangle(x - 2, y + 1, 4, 15, (Color){47,25,17,255});
    DrawRectangle(x - 1, y + 1, 2, 13, (Color){118,63,25,255});
    DrawRectangle(x - 4, y - 1, 8, 3, (Color){59,52,42,255});
    int flicker = (int)(time * 12 + index) % 3;
    DrawRectangle(x - 4, y - 12, 8, 11, (Color){235,71,11,255});
    DrawRectangle(x - 2, y - 17 - flicker, 3, 15, (Color){255,129,14,255});
    DrawRectangle(x + 2, y - 11 - flicker, 2, 8, (Color){255,156,19,255});
    DrawRectangle(x - 2, y - 9, 5, 8, (Color){255,226,85,255});
    DrawRectangle(x, y - 7, 2, 6, (Color){255,255,208,255});
    for (int i = 0; i < 3; i++)
    {
        float phase = fmodf(time * 0.9f + i * 0.33f + index * 0.12f, 1);
        DrawRectangle(x + (int)(sinf(phase*9 + i)*4), y - 14 - (int)(phase*20), 1, 2,
                      Fade(gold, (1-phase)*0.8f));
    }
}

static void drawPlayer(const GraphicGame *g)
{
    float t = g->step * g->step * (3 - 2 * g->step);
    float col = g->fromCol + (g->playerCol - g->fromCol)*t;
    float row = g->fromRow + (g->playerRow - g->fromRow)*t;
    int x = BOARD_X + (int)roundf(col*TILE) + 9;
    int y = BOARD_Y + (int)roundf(row*TILE) + 16;
    int walking = g->step < 1;
    int bob = walking ? (int)roundf(sinf(g->step*PI)*2) : (int)(sinf(g->time*2.5f)>0.7f);
    int foot = walking ? (int)roundf(sinf(g->step*2*PI)*2) : 0;
    DrawEllipse(x, y, 8, 3, (Color){0,0,0,125});
    /* Split the existing sprite into torso and feet for a visible walk cycle. */
    float flip = g->facing < 0 ? -1.0f : 1.0f;
    sprite((Rectangle){1034,208,222*flip,270}, x-12, y-32-bob, 24, 29, WHITE);
    sprite((Rectangle){1034,478,111,55}, x-12, y-4-foot, 12, 6, WHITE);
    sprite((Rectangle){1145,478,111,55}, x, y-4+foot, 12, 6, WHITE);
}

static void drawTreasure(const GraphicGame *g)
{
    int x = BOARD_X + g->treasureCol*TILE + 9;
    int y = BOARD_Y + g->treasureRow*TILE + 14;
    float opening = clamp01(g->winTime * 2);
    DrawCircleGradient(x, y - 3, 26, (Color){234,160,38,30}, BLANK);
    /* Lid lifts above the chest; the base stays fixed. */
    sprite((Rectangle){1415,421,280,113}, x-13, y-7, 26, 11, WHITE);
    if (opening > 0) DrawRectangle(x-11, y-7, 22, 4, (Color){255,229,131,255});
    sprite((Rectangle){1415,273,280,148}, x-13, y-21-opening*8, 26, 14-opening*4, WHITE);
    for (int i = 0; i < 4; i++)
    {
        float phase = fmodf(g->time * 0.45f + i * 0.25f, 1);
        int sx = x + (int)(sinf(i*2.1f + g->time*0.4f)*15);
        int sy = y - 4 - (int)(phase*26);
        Color light = Fade(gold, sinf(phase*PI)*0.8f);
        DrawLine(sx-1,sy,sx+1,sy,light); DrawLine(sx,sy-1,sx,sy+1,light);
    }
}

static void drawKey(int x, int y, const char *key, int pressed)
{
    Color edge = pressed ? gold : (Color){128,129,145,255};
    DrawRectangle(x+2,y,18,22,(Color){24,29,39,255});
    DrawRectangle(x,y+2,22,18,(Color){24,29,39,255});
    DrawRectangleLines(x+2,y,18,22,edge);
    DrawLine(x,y+2,x,y+19,edge); DrawLine(x+21,y+2,x+21,y+19,edge);
    centered(textFont,key,x,y+1,22,22,pressed ? gold : parchment);
}

static void drawCompass(int x, int y, float time)
{
    DrawCircleLines(x,y,28,bronze);
    DrawCircleLines(x,y,29,(Color){49,36,27,255});
    for (int i=0; i<8; i++)
    {
        float a = i*PI/4;
        float length = i%2 ? 24.0f : 38.0f;
        Vector2 tip = {x+sinf(a)*length,y-cosf(a)*length};
        Vector2 side = {x+cosf(a)*4,y+sinf(a)*4};
        Vector2 other = {x-cosf(a)*3,y-sinf(a)*3};
        DrawTriangle((Vector2){(float)x,(float)y}, side, tip, (Color){117,82,43,255});
        DrawTriangle((Vector2){(float)x,(float)y}, tip, other, (Color){59,43,30,255});
    }
    DrawCircle(x,y,3,(Color){178,121,58,255});
    DrawCircle(x,y,1,Fade(gold,0.7f+0.2f*sinf(time)));
}

static void drawSidebar(const GraphicGame *g)
{
    const int x=565, w=146;
    frame(x,9,w,445);
    centered(titleFont,"Labyrinth-",x,22,w,37,gold);
    centered(titleFont,"Spiel",x,50,w,37,gold);
    DrawLine(x+10,86,x+w-11,86,bronze);
    DrawRectangle(x+w/2-2,84,4,5,bronze);
    frame(x+8,102,w-16,70);
    label(textFont,"Z\xc3\xbcge:",x+21,110,21,parchment);
    centered(textFont,TextFormat("%d",g->moves),x,127,w,35,(Color){237,216,181,255});
    frame(x+8,183,w-16,137);
    label(textFont,"Steuerung:",x+20,195,20,parchment);
    const int keys[] = {KEY_W,KEY_A,KEY_S,KEY_D};
    const char *names[] = {"W","A","S","D"};
    for(int i=0;i<4;i++) drawKey(x+20+i*28,222,names[i],IsKeyDown(keys[i]));
    label(textFont,"bewegen",x+23,246,20,parchment);
    drawKey(x+20,278,"Q",IsKeyDown(KEY_Q));
    label(textFont,"beenden",x+52,280,20,parchment);
    drawCompass(x+w/2,371,g->time);
    centered(textFont,"Finde die Truhe!",x,419,w,17,(Color){159,157,168,255});
}

static void drawWinScreen(const GraphicGame *g)
{
    float fade = clamp01((g->winTime-0.6f)*3);
    if (fade <= 0) return;
    DrawRectangle(BOARD_X,BOARD_Y-4,COLS*TILE,ROWS*TILE+8,Fade(BLACK,fade*0.63f));
    frame(103,171,352,122);
    centered(titleFont,"Schatz gefunden!",103,185,352,32,gold);
    centered(textFont,TextFormat("Gewonnen in %d Z\xc3\xbcgen",g->moves),103,224,352,21,parchment);
    centered(textFont,"R  Neues Spiel     Q  Beenden",103,259,352,20,parchment);
}

static void drawScene(const GraphicGame *g)
{
    ClearBackground(ink);
    drawLabyrinth(g);
    drawTreasure(g);
    drawPlayer(g);
    for(int i=0;i<TORCH_COUNT;i++)
        drawTorch(BOARD_X+g->torchCol[i]*TILE+9,BOARD_Y+g->torchRow[i]*TILE+7,g->time,i);
    drawSidebar(g);
    frame(8,466,704,37);
    label(textFont,g->blockedTime>0 ? "Hier ist eine Mauer. Such dir einen anderen Weg." :
          "Ein dunkles Labyrinth. Irgendwo hier wartet ein Schatz ...",21,475,18,
          g->blockedTime>0 ? gold : parchment);
    label(textFont,"Viel Erfolg!",619,475,18,parchment);
    if(g->won) drawWinScreen(g);
}

void startGraphicGame(void)
{
    GraphicGame game;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1440,1020,"Labyrinth-Spiel");
    if(!IsWindowReady()) { printf("Grafikfenster konnte nicht geoeffnet werden.\n"); return; }
    SetWindowMinSize(720,510);
    int monitor=GetCurrentMonitor();
    if(GetMonitorHeight(monitor)<1120) SetWindowSize(1080,765);
    SetTargetFPS(60);
    if(!loadGraphics())
    {
        printf("Grafikdatei fehlt: assets/dungeon_sprites.png\n");
        CloseWindow(); return;
    }
    RenderTexture2D canvas=LoadRenderTexture(VIEW_WIDTH,VIEW_HEIGHT);
    SetTextureFilter(canvas.texture,TEXTURE_FILTER_POINT);
    startRound(&game,1);
    while(!WindowShouldClose() && !IsKeyPressed(KEY_Q))
    {
        updateGame(&game,fminf(GetFrameTime(),0.05f),readMovement());
        BeginTextureMode(canvas);
        drawScene(&game);
        EndTextureMode();
        float scale=fminf((float)GetScreenWidth()/VIEW_WIDTH,(float)GetScreenHeight()/VIEW_HEIGHT);
        Rectangle target={(GetScreenWidth()-VIEW_WIDTH*scale)/2,
                          (GetScreenHeight()-VIEW_HEIGHT*scale)/2,VIEW_WIDTH*scale,VIEW_HEIGHT*scale};
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(canvas.texture,(Rectangle){0,0,VIEW_WIDTH,-VIEW_HEIGHT},target,(Vector2){0,0},0,WHITE);
        EndDrawing();
    }
    UnloadRenderTexture(canvas);
    unloadGraphics();
    CloseWindow();
}
