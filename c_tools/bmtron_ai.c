#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 40
#define HEIGHT 20

/*
gcc bmtron_ai.c -o bmtron_ai -lncurses
./bmtron_ai

*/

#define PLAYER1_HEAD 'O'
#define PLAYER2_HEAD 'X'
#define TRAIL '*'
#define EMPTY ' '
#define WALL '#'

typedef struct {
    int x, y;
    int dx, dy;
    char head;
} Player;

void init_player(Player* player, int x, int y, int dx, int dy, char head) {
    player->x = x;
    player->y = y;
    player->dx = dx;
    player->dy = dy;
    player->head = head;
}

void move_player(Player* player) {
    player->x += player->dx;
    player->y += player->dy;
}

int is_collision(Player* player, char grid[HEIGHT][WIDTH]) {
    return player->x < 0 || player->x >= WIDTH || player->y < 0 || player->y >= HEIGHT || grid[player->y][player->x] != EMPTY;
}

void draw_grid(char grid[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            mvaddch(y, x, grid[y][x]);
        }
    }
    refresh();
}

void setup_grid(char grid[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                grid[y][x] = WALL;  // Set boundaries
            } else {
                grid[y][x] = EMPTY;
            }
        }
    }
}

void ai_move(Player* ai, char grid[HEIGHT][WIDTH]) {
    // Simple AI to avoid walls and trails
    int new_dx = ai->dx, new_dy = ai->dy;

    // Check forward direction
    int next_x = ai->x + ai->dx;
    int next_y = ai->y + ai->dy;

    if (next_x < 0 || next_x >= WIDTH || next_y < 0 || next_y >= HEIGHT || grid[next_y][next_x] != EMPTY) {
        // Try turning right
        new_dx = -ai->dy;
        new_dy = ai->dx;

        next_x = ai->x + new_dx;
        next_y = ai->y + new_dy;

        if (next_x < 0 || next_x >= WIDTH || next_y < 0 || next_y >= HEIGHT || grid[next_y][next_x] != EMPTY) {
            // Try turning left if right turn is not possible
            new_dx = ai->dy;
            new_dy = -ai->dx;
        }
    }

    // Update AI direction
    ai->dx = new_dx;
    ai->dy = new_dy;

    move_player(ai);
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    timeout(100); // Set delay for player movement
    keypad(stdscr, TRUE);

    char grid[HEIGHT][WIDTH];
    setup_grid(grid);

    Player player1, ai;
    init_player(&player1, 5, HEIGHT / 2, 1, 0, PLAYER1_HEAD);
    init_player(&ai, WIDTH - 6, HEIGHT / 2, -1, 0, PLAYER2_HEAD);

    int ch;
    int running = 1;
    while (running) {
        // Draw players
        grid[player1.y][player1.x] = player1.head;
        grid[ai.y][ai.x] = ai.head;

        draw_grid(grid);

        ch = getch();
        switch (ch) {
            // Player 1 controls (WASD)
            case KEY_UP: player1.dx = 0; player1.dy = -1; break;
            case KEY_DOWN: player1.dx = 0; player1.dy = 1; break;
            case KEY_LEFT: player1.dx = -1; player1.dy = 0; break;
            case KEY_RIGHT: player1.dx = 1; player1.dy = 0; break;
        }

        // Leave a trail and move players
        grid[player1.y][player1.x] = TRAIL;
        grid[ai.y][ai.x] = TRAIL;

        move_player(&player1);
        ai_move(&ai, grid);

        // Check for collisions
        if (is_collision(&player1, grid)) {
            mvprintw(HEIGHT + 1, 0, "AI wins!");
            printf("AI wins!");
            running = 0;
        }
        if (is_collision(&ai, grid)) {
            mvprintw(HEIGHT + 2, 0, "Player 1 wins!");
            printf("Player 1 wins!");
            running = 0;
        }
    }

    getch();
    endwin();
    return 0;
}
