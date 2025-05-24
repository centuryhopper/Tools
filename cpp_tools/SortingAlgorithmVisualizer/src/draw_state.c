#include "../include/draw_state.h"
#include "../include/configs.h"
#include "../include/utils.h"


void draw_state_with_color(const int *arr, int red, int blue, int green, Color color)
{
    // ClearBackground(BLACK);

    // BeginDrawing();

    int barWidth = SCREEN_WIDTH / ELEMENT_COUNT;
    // Find the maximum value in the array for dynamic scaling
    int maxVal = getMaxValue(arr, ELEMENT_COUNT);

    for (int i = 0; i < ELEMENT_COUNT; ++i)
    {
        if (i == red)
            color = RED;
        else if (i == blue)
            color = BLUE;
        else if (i == green)
            color = GREEN;

        // Scale height based on max value so tallest bar reaches the top
        float scale = (float)(SCREEN_HEIGHT * 0.8f) / maxVal;
        int posX = i * barWidth;
        int barHeight = arr[i] * scale;
        int posY = SCREEN_HEIGHT - barHeight;
        DrawRectangle(posX, posY, barWidth - 1, barHeight, color);
    }

    // EndDrawing();
    // WaitTime(0.01);
}

void draw_state(const int *arr, int red, int blue, int green)
{
    // ClearBackground(BLACK);

    // BeginDrawing();

    int barWidth = SCREEN_WIDTH / ELEMENT_COUNT;
    // Find the maximum value in the array for dynamic scaling
    int maxVal = getMaxValue(arr, ELEMENT_COUNT);

    for (int i = 0; i < ELEMENT_COUNT; ++i)
    {
        Color color = YELLOW;
        if (i == red)
            color = RED;
        else if (i == blue)
            color = BLUE;
        else if (i == green)
            color = GREEN;

        // Scale height based on max value so tallest bar reaches the top
        float scale = (float)(SCREEN_HEIGHT * 0.8f) / maxVal;
        int posX = i * barWidth;
        int barHeight = arr[i] * scale;
        int posY = SCREEN_HEIGHT - barHeight;
        DrawRectangle(posX, posY, barWidth - 1, barHeight, color);
    }

    // EndDrawing();
    // WaitTime(0.01);
}
