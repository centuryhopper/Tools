#include "raylib.h"
#include "../include/configs.h"
#include "../include/draw_state.h"

/*
for (int i = size - 1; i >= 0; i--)
    {
        bool swapped = false;
        for (int j = 1; j <= i; j++)
        {
            if (arr[j] < arr[j - 1])
            {
                int tmp = arr[j];
                arr[j] = arr[j - 1];
                arr[j - 1] = tmp;
                swapped = true;
            }
        }
        if (!swapped)
            break; // list is sorted
    }
*/

// Bubble Sort with per-frame visualization
void bubbleSort(int *arr)
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

            draw_state(arr, i, j, -1);
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

void bubbleSortRaw(int *arr, int size)
{
    if (size < 2)
        return;
    for (int i = size - 1; i >= 0; i--)
    {
        bool swapped = false;
        for (int j = 1; j <= i; j++)
        {
            if (arr[j] < arr[j - 1])
            {
                int tmp = arr[j];
                arr[j] = arr[j - 1];
                arr[j - 1] = tmp;
                swapped = true;
            }
        }
        if (!swapped)
            break; // list is sorted
    }
}