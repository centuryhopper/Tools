#include "raylib.h"
#include "../include/configs.h"
#include "../include/utils.h"
#include "../include/bubble_sort.h"
#include <stdlib.h>
#include <stdio.h>

void cleanUpBubbleSortState(BubbleSortState **state)
{
    if (*state)
    {
        free(*state);
        *state = NULL;
    }
}

void initializeBubbleSortState(BubbleSortState **state)
{
    if (*state)
    {
        cleanUpBubbleSortState(state);
    }
    *state = malloc(sizeof(BubbleSortState));
    if (!(*state))
    {
        printf("bubble sort malloc failed\n");
        return;
    }

    (*state)->i = ELEMENT_COUNT - 1;
    (*state)->j = 1;
    (*state)->swapped = 0;
    (*state)->sorting = 1;
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