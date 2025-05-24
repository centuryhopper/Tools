#include "raylib.h"
#include "../include/configs.h"
#include "../include/draw_state.h"
#include "../include/bubble_sort.h"
#include <stdlib.h>



BubbleSortState* cleanUpBubbleSortState(BubbleSortState* state)
{
    if (state)
    {
        free(state);
    }
    return NULL;
}

BubbleSortState* initializeBubbleSortState(BubbleSortState* state, BubbleSortState values)
{
    if (state)
    {
        state = cleanUpBubbleSortState(state);
    }
    state = malloc(sizeof(BubbleSortState));
    if (!state)
    {
        return NULL;
    }
    state->i = values.i;
    state->j = values.j;
    state->swapped = values.swapped;
    state->sorting = values.sorting;
    return state;
}

// Bubble Sort with per-frame visualization
void bubbleSort(int *arr, BubbleSortState *state)
{
    if (state->j <= state->i)
    {
        if (arr[state->j] < arr[state->j - 1])
        {
            int temp = arr[state->j];
            arr[state->j] = arr[state->j - 1];
            arr[state->j - 1] = temp;
            state->swapped = 1;
        }

        draw_state(arr, state->i, state->j, -1);
        state->j++;
    }
    else
    {
        if (!state->swapped)
            state->sorting = 0;

        state->swapped = 0;
        state->i--;
        state->j = 1;
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
            // draw_state(arr, i, j, -1);
        }
        if (!swapped)
            break; // list is sorted
    }
}