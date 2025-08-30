#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <conio.h> // Windows专用，Linux可用ncurses库替换

#define ROWS 12
#define COLS 8
#define COLORS 4
#define EMPTY 0

typedef struct {
    int color;
    bool moving;
    float x, y;
    float dx, dy;
} Bubble;

int grid[ROWS][COLS] = {0};
Bubble shooter;
int score = 0;

void init_grid() {
    srand(time(0));
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < COLS; ++j)
            grid[i][j] = rand() % COLORS + 1;
    }
}

void draw_grid() {
    system("cls"); // Windows清屏，Linux用"clear"
    printf("泡泡龙小游戏 (方向键控制，空格发射，Q退出)\n");
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            if (grid[i][j] == EMPTY)
                printf(" . ");
            else
                printf(" %c ", 'A' + grid[i][j] - 1);
        }
        printf("\n");
    }
    printf("得分: %d\n", score);
    printf("发射器位置: (%.1f, %.1f) 颜色:%c\n", shooter.x, shooter.y, 'A' + shooter.color - 1);
}

void new_shooter() {
    shooter.x = COLS / 2;
    shooter.y = ROWS - 1;
    shooter.color = rand() % COLORS + 1;
    shooter.moving = false;
    shooter.dx = 0;
    shooter.dy = 0;
}

bool in_bounds(int x, int y) {
    return x >= 0 && x < COLS && y >= 0 && y < ROWS;
}

void fire_bubble(float angle_deg) {
    float angle_rad = angle_deg * M_PI / 180.0;
    shooter.dx = cos(angle_rad) * 0.5;
    shooter.dy = -sin(angle_rad) * 0.5;
    shooter.moving = true;
}

void move_bubble() {
    if (!shooter.moving) return;
    float nx = shooter.x + shooter.dx;
    float ny = shooter.y + shooter.dy;
    if (nx < 0) {
        nx = 0; shooter.dx = -shooter.dx;
    }
    if (nx >= COLS - 1) {
        nx = COLS - 1; shooter.dx = -shooter.dx;
    }
    if (ny < 0 || grid[(int)ny][(int)nx] != EMPTY) {
        // 碰到顶部或泡泡，停下
        int gx = (int)(nx + 0.5);
        int gy = (int)(ny + 0.5);
        if (gy < 0) gy = 0;
        if (gx < 0) gx = 0;
        if (gx >= COLS) gx = COLS - 1;
        if (gy >= ROWS) gy = ROWS - 1;
        grid[gy][gx] = shooter.color;
        shooter.moving = false;
        // 检查消除
        // flood fill找同色泡泡
        bool visited[ROWS][COLS] = {0};
        int stackx[ROWS*COLS], stacky[ROWS*COLS], top = 0;
        int count = 0;
        int cur_color = shooter.color;
        stackx[top] = gx;
        stacky[top++] = gy;
        visited[gy][gx] = true;
        while (top > 0) {
            int x = stackx[--top], y = stacky[top];
            count++;
            for (int d = 0; d < 4; ++d) {
                int nx = x + (d==0)-(d==1); // 左右
                int ny = y + (d==2)-(d==3); // 上下
                if (in_bounds(nx, ny) && !visited[ny][nx] && grid[ny][nx] == cur_color) {
                    stackx[top] = nx;
                    stacky[top++] = ny;
                    visited[ny][nx] = true;
                }
            }
        }
        if (count >= 3) { // 至少3个消除
            for (int i = 0; i < ROWS; ++i)
                for (int j = 0; j < COLS; ++j)
                    if (visited[i][j]) grid[i][j] = EMPTY;
            score += count;
        }
        new_shooter();
    } else {
        shooter.x = nx;
        shooter.y = ny;
    }
}

int main() {
    init_grid();
    new_shooter();
    float angle = 90.0f; // 90度向上
    draw_grid();
    while (1) {
        if (!shooter.moving) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 'q' || ch == 'Q') break;
                if (ch == 75) angle -= 10; // 左箭头
                if (ch == 77) angle += 10; // 右箭头
                if (ch == ' ') fire_bubble(angle);
            }
        }
        move_bubble();
        draw_grid();
        if (grid[ROWS-1][COLS/2] != EMPTY) {
            printf("游戏结束！\n");
            break;
        }
        // 控制速度
        struct timespec ts = {0, 100 * 1000000}; // 100ms
        nanosleep(&ts, NULL);
    }
    return 0;
}
