//编译参数 g++ -D_UNICODE -DUNICODE -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ldxguid -ld3d11 -lxinput -ldinput8 -ld2d1 -ldwrite -lwindowscodecs -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 -ldsound -lsetupapi -w Snake.cpp
#include <windows.h>
#include <time.h>

#define GRID_SIZE 20
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TIMER_ID 1

POINT snake[100];
int length = 5;
POINT food;
bool gameOver = false;
int direction = VK_RIGHT;

void draw(HDC hdc) {
    RECT rect;
    HBRUSH hBrush, oldBrush;
    HPEN hPen, oldPen;

    // 绘制背景
    hBrush = CreateSolidBrush(RGB(240, 240, 240));
    FillRect(hdc, &rect, hBrush);
    DeleteObject(hBrush);

    // 绘制蛇
    hBrush = CreateSolidBrush(RGB(0, 128, 0));
    for (int i = 0; i < length; i++) {
        Rectangle(hdc, snake[i].x, snake[i].y, snake[i].x + GRID_SIZE, snake[i].y + GRID_SIZE);
    }
    DeleteObject(hBrush);

    // 绘制食物
    hBrush = CreateSolidBrush(RGB(255, 0, 0));
    Rectangle(hdc, food.x, food.y, food.x + GRID_SIZE, food.y + GRID_SIZE);
    DeleteObject(hBrush);
}

void update() {
    for (int i = length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    switch (direction) {
    case VK_UP:
        snake[0].y -= GRID_SIZE;
        break;
    case VK_DOWN:
        snake[0].y += GRID_SIZE;
        break;
    case VK_LEFT:
        snake[0].x -= GRID_SIZE;
        break;
    case VK_RIGHT:
        snake[0].x += GRID_SIZE;
        break;
    }

    // 检查是否吃到食物
    if (snake[0].x == food.x && snake[0].y == food.y) {
        length++;
        food.x = (rand() % (SCREEN_WIDTH / GRID_SIZE)) * GRID_SIZE;
        food.y = (rand() % (SCREEN_HEIGHT / GRID_SIZE)) * GRID_SIZE;
    }

    // 检查碰撞
    if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT) {
        gameOver = true;
    }

    for (int i = 1; i < length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameOver = true;
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;

    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, TIMER_ID, 100, NULL);
        srand(time(NULL));
        snake[0].x = 100;
        snake[0].y = 100;
        food.x = 200;
        food.y = 200;
        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        draw(hdc);
        EndPaint(hwnd, &ps);
        break;
    case WM_TIMER:
        update();
        if (gameOver) {
            KillTimer(hwnd, TIMER_ID);
            MessageBox(hwnd, L"游戏结束！", L"提示", MB_OK);
            PostQuitMessage(0);
        }
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    case WM_KEYDOWN:
        direction = (int)wParam;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpszClassName = L"SnakeGame";
    wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
    wc.lpfnWndProc = WndProc;
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"SnakeGame", L"贪吃蛇游戏", WS_OVERLAPPEDWINDOW, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}