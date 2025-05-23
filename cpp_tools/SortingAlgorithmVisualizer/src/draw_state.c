#include "../include/draw_state.h"

#define ELEMENT_COUNT 100
#define SCREEN_HEIGHT 1200
#define BAR_WIDTH 20

void draw_state(const int* arr, int red, int blue)
{
    ClearBackground(BLACK);

    for (int i = 0; i < ELEMENT_COUNT; ++i)
    {
        Color color = WHITE;
        if (i == red) color = RED;
        else if (i == blue) color = BLUE;

        int posX = i * BAR_WIDTH;
        int barHeight = arr[i] * 10;
        DrawRectangle(posX, SCREEN_HEIGHT - barHeight, BAR_WIDTH, barHeight, color);
    }
}
