#include "raylib.h"
#include <limits.h>
#include "../include/configs.h"
#include "../include/draw_state.h"

static int quickSortPartition(int *arr, int size, int lo, int hi)
{
    // if (hi >= size)
    // {
    //     printf("hi exceeded bounds\n");
    // }
    // if (lo < 0)
    // {
    //     printf("lo is negative\n");
    // }
    int pivot = lo;
    int i = lo;
    int j = hi;
    int left, right;
    while (i < j)
    {
        do
        {
            i += 1;
            left = arr[i];
            BeginDrawing();
            draw_state(arr, i, -1, pivot);
            EndDrawing();
            WaitTime(0.05);
        } while (left <= arr[pivot]);
        do
        {
            j -= 1;
            right = (j < size) ? arr[j] : INT_MAX;
            BeginDrawing();
            draw_state(arr, -1, j, pivot);
            EndDrawing();
            WaitTime(0.05);
        } while (right > arr[pivot]);

        if (i < j)
        {
            int tmp = arr[j];
            arr[j] = arr[i];
            arr[i] = tmp;
        }
    }
    int tmp = arr[j];
    arr[j] = arr[pivot];
    arr[pivot] = tmp;
    return j;
}

static void runner(int *arr, int size, int lo, int hi)
{
    if (lo >= hi)
    {
        return;
    }
    int partitionIdx = quickSortPartition(arr, size, lo, hi);
    runner(arr, size, lo, partitionIdx);
    runner(arr, size, partitionIdx + 1, hi);
}

void quickSort(int *arr)
{
    runner(arr, ELEMENT_COUNT, 0, ELEMENT_COUNT);
}
