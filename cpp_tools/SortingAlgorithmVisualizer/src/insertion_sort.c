#include "raylib.h"
#include "../include/configs.h"
#include "../include/draw_state.h"

/*
    for (int i = 0; i < size; i++)
    {
        for (int j = i + 1; j > 0; j--)
        {
            if (arr[j] < arr[j - 1])
            {
                int tmp = arr[j - 1];
                arr[j - 1] = arr[j];
                arr[j] = tmp;
            }
        }
    }
*/

// Insertion Sort with per-frame visualization
void insertionSort(int *arr)
{
    int i = 0;
    int j = i + 1;
    // make sure j doesn't overshoot out of the bounds of the array
    for (; !WindowShouldClose() && i < ELEMENT_COUNT - 1;)
    {
        BeginDrawing();
        if (j > 0)
        {
            if (arr[j] < arr[j - 1])
            {
                int tmp = arr[j - 1];
                arr[j - 1] = arr[j];
                arr[j] = tmp;
            }
            draw_state(arr, i, j, -1);
            j -= 1;
        }
        else
        {
            i += 1;
            j = i + 1;
        }

        EndDrawing();
        WaitTime(WAIT_TIME);
    }
}

void insertionSortRaw(int *arr, int size)
{
    if (size < 2)
        return;
    // make sure j doesn't overshoot
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j > 0; j--)
        {
            if (arr[j] < arr[j - 1])
            {
                int tmp = arr[j - 1];
                arr[j - 1] = arr[j];
                arr[j] = tmp;
            }
        }
    }
}