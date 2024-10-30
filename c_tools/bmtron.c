#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

/*

sudo apt install libncurses5-dev

gcc bmtron.c -o bmtron -lncurses
./bmtron

 */

#define WIDTH 40
#define HEIGHT 20

#define PLAYER1_HEAD 'O'
#define PLAYER2_HEAD 'X'
#define TRAIL '*'
#define EMPTY ' '

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
            grid[y][x] = EMPTY;
        }
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    timeout(100); // Set delay for player movement
    keypad(stdscr, TRUE);

    char grid[HEIGHT][WIDTH];
    setup_grid(grid);

    Player player1, player2;
    init_player(&player1, 5, HEIGHT / 2, 1, 0, PLAYER1_HEAD);
    init_player(&player2, WIDTH - 6, HEIGHT / 2, -1, 0, PLAYER2_HEAD);

    int ch;
    int running = 1;
    while (running) {
        // Draw players
        grid[player1.y][player1.x] = player1.head;
        grid[player2.y][player2.x] = player2.head;

        draw_grid(grid);

        ch = getch();
        switch (ch) {
            // Player 1 controls (WASD)
            case 'w': player1.dx = 0; player1.dy = -1; break;
            case 's': player1.dx = 0; player1.dy = 1; break;
            case 'a': player1.dx = -1; player1.dy = 0; break;
            case 'd': player1.dx = 1; player1.dy = 0; break;
            // Player 2 controls (Arrow keys)
            case KEY_UP: player2.dx = 0; player2.dy = -1; break;
            case KEY_DOWN: player2.dx = 0; player2.dy = 1; break;
            case KEY_LEFT: player2.dx = -1; player2.dy = 0; break;
            case KEY_RIGHT: player2.dx = 1; player2.dy = 0; break;
        }

        // Leave a trail and move players
        grid[player1.y][player1.x] = TRAIL;
        grid[player2.y][player2.x] = TRAIL;
        
        move_player(&player1);
        move_player(&player2);

        // Check for collisions
        if (is_collision(&player1, grid)) {
            mvprintw(HEIGHT + 1, 0, "Player 2 wins!");
            running = 0;
        }
        if (is_collision(&player2, grid)) {
            mvprintw(HEIGHT + 2, 0, "Player 1 wins!");
            running = 0;
        }
    }

    getch();
    endwin();
    return 0;
}
