
#include <LCUI.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 20
#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 400
#define TIMER_INTERVAL 200

typedef struct _Point {
    int x, y;
} Point;

Point snake[100];
int length = 5;
Point food;
int direction = 1; /* 1: right, 2: down, 3: left, 4: up */

void DrawSnake(LCUI_PaintContext paint) {
    LCUI_Rect rect;
    rect.width = GRID_SIZE - 2;
    rect.height = GRID_SIZE - 2;
    LCUI_SetColor(paint, RGB(0, 128, 0));
    for (int i = 0; i < length; i++) {
        rect.x = snake[i].x * GRID_SIZE + 1;
        rect.y = snake[i].y * GRID_SIZE + 1;
        LCUI_DrawRect(paint, rect, 0, 0, 0, 0);
    }
}

void DrawFood(LCUI_PaintContext paint) {
    LCUI_Rect rect;
    rect.x = food.x * GRID_SIZE + 1;
    rect.y = food.y * GRID_SIZE + 1;
    rect.width = GRID_SIZE - 2;
    rect.height = GRID_SIZE - 2;
    LCUI_SetColor(paint, RGB(255, 0, 0));
    LCUI_DrawRect(paint, rect, 0, 0, 0, 0);
}

void DrawGame(LCUI_Widget w, LCUI_PaintContext paint) {
    LCUI_Rect rect;
    rect.width = SCREEN_WIDTH;
    rect.height = SCREEN_HEIGHT;
    LCUI_SetColor(paint, RGB(240, 240, 240));
    LCUI_FillRect(paint, rect, 0, 0, 0, 0);
    DrawSnake(paint);
    DrawFood(paint);
}

void UpdateGame(void *arg) {
    int i;
    Point newHead = snake[0];
    switch (direction) {
    case 1:
        newHead.x++;
        break;
    case 2:
        newHead.y++;
        break;
    case 3:
        newHead.x--;
        break;
    case 4:
        newHead.y--;
        break;
    }
    for (i = length - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }
    snake[0] = newHead;
    if (newHead.x == food.x && newHead.y == food.y) {
        length++;
        food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
        food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
    }
    if (newHead.x < 0 || newHead.x >= SCREEN_WIDTH / GRID_SIZE ||
        newHead.y < 0 || newHead.y >= SCREEN_HEIGHT / GRID_SIZE) {
        LCUI_PostTask(0, LCUI_Quit);
    }
    for (i = 1; i < length; ++i) {
        if (newHead.x == snake[i].x && newHead.y == snake[i].y) {
            LCUI_PostTask(0, LCUI_Quit);
        }
    }
    LCUI_MainLoop();
}

void OnKeyDown(LCUI_WidgetEvent e, void *arg) {
    switch (e->key.keysym) {
    case LCUI_KEY_RIGHT:
        if (direction != 3) {
            direction = 1;
        }
        break;
    case LCUI_KEY_DOWN:
        if (direction != 4) {
            direction = 2;
        }
        break;
    case LCUI_KEY_LEFT:
        if (direction != 1) {
            direction = 3;
        }
        break;
    case LCUI_KEY_UP:
        if (direction != 2) {
            direction = 4;
        }
        break;
    }
}

int main(void) {
    LCUI_Init();
    LCUI_Widget root, game;
    srand(time(NULL));
    root = LCUIWidget_GetRoot();
    game = LCUI_CreateWidget("game");
    LCUIWidget_BindEvent(game, "paint", DrawGame, NULL, NULL);
    LCUIWidget_BindEvent(game, "update", UpdateGame, NULL, NULL);
    LCUIWidget_BindEvent(root, "keydown", OnKeyDown, NULL, NULL);
    LCUIWidget_Append(root, game);
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
    LCUI_SetInterval(UpdateGame, TIMER_INTERVAL, NULL);
    LCUI_Main();
    LCUI_Destroy();
    return 0;
}