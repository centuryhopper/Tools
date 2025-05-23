#include "raylib.h"
#include "../include/configs.h"
#include "../include/draw_state.h"

/*
    for (int i = 0; i < size; i++)
    {
        assume ith index is the min so far
        int minIdx = i;
        for (int j = i + 1; j < size; j++)
        {
            if (arr[minIdx] > arr[j])
            {
                minIdx = j;
            }
        }
        int tmp = arr[i];
        arr[i] = arr[minIdx];
        arr[minIdx] = tmp;
    }
*/

// Selection Sort with per-frame visualization
void selectionSort(int *arr)
{
    int i = 0;
    int j = i + 1;
    int minIdx = i;
    // make sure j doesn't overshoot out of the bounds of the array
    for (; !WindowShouldClose() && i < ELEMENT_COUNT - 1;)
    {
        BeginDrawing();
        if (j < ELEMENT_COUNT)
        {
            if (arr[minIdx] > arr[j])
            {
                minIdx = j;
            }
            draw_state(arr, i, j, -1);
            j += 1;
        }
        else
        {
            int tmp = arr[i];
            arr[i] = arr[minIdx];
            arr[minIdx] = tmp;
            i += 1;
            minIdx = i;
            j = i + 1;
        }

        EndDrawing();
        WaitTime(0.01);
    }
}
