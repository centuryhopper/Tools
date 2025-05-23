#include "raylib.h"
#include "../include/configs.h"
#include "../include/draw_state.h"

// Merge function with visualization
static void mergeyMerge(int *arr, int left, int mid, int right)
{
    int tmpArraySize = right - left + 1;
    int tmp[tmpArraySize];
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right)
    {
        if (WindowShouldClose()) return;

        if (arr[i] <= arr[j])
            tmp[k++] = arr[i++];
        else
            tmp[k++] = arr[j++];

        BeginDrawing();
        draw_state(arr, i, j, -1);
        EndDrawing();
        WaitTime(0.01);
    }

    while (i <= mid)
    {
        if (WindowShouldClose()) return;
        tmp[k++] = arr[i++];
    }

    while (j <= right)
    {
        if (WindowShouldClose()) return;
        tmp[k++] = arr[j++];
    }

    for (int p = 0; p < tmpArraySize; ++p)
    {
        if (WindowShouldClose()) return;

        arr[left + p] = tmp[p];

        BeginDrawing();
        draw_state(arr, left + p, -1, -1);
        EndDrawing();
        WaitTime(0.01);
    }
}

// Recursive runner with exit check
void runner(int* arr, int left, int right)
{
    if (WindowShouldClose()) return;
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    runner(arr, left, mid);
    if (WindowShouldClose()) return;

    runner(arr, mid + 1, right);
    if (WindowShouldClose()) return;

    mergeyMerge(arr, left, mid, right);
}

// Public interface
void mergeSort(int *arr)
{
    runner(arr, 0, ELEMENT_COUNT - 1);
}

