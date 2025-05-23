#include "raylib.h"
#include "../include/configs.h"
#include "../include/draw_state.h"


// Bubble Sort with per-frame visualization
void bubbleSort(int* arr)
{
    int i = ELEMENT_COUNT - 1;
    int j = 1;
    int swapped = 0;
    int sorting = 1;

    while (!WindowShouldClose() && sorting)
    {
        BeginDrawing();

        if (j <= i)
        {
            if (arr[j] < arr[j - 1])
            {
                int temp = arr[j];
                arr[j] = arr[j - 1];
                arr[j - 1] = temp;
                swapped = 1;
            }

            draw_state(arr, i, j);
            j++;
        }
        else
        {
            if (!swapped)
                sorting = 0;

            swapped = 0;
            i--;
            j = 1;
        }

        EndDrawing();
        WaitTime(0.01);
    }
}
