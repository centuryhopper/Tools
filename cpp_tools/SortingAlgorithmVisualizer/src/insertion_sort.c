#include "raylib.h"
#include <stdlib.h>
#include "../include/configs.h"
#include "../include/draw_state.h"
#include "../include/insertion_sort.h"

InsertionSortState *cleanUpInsertionSortState(InsertionSortState *state)
{
    if (state)
    {
        free(state);
    }
    return NULL;
}

InsertionSortState *initializeInsertionSortState(InsertionSortState *state, InsertionSortState values)
{
    if (state)
    {
        state = cleanUpInsertionSortState(state);
    }
    state = malloc(sizeof(InsertionSortState));
    if (!state)
    {
        return NULL;
    }
    state->i = values.i;
    state->j = values.j;
    return state;
}

// Insertion Sort with per-frame visualization
void insertionSort(int *arr, InsertionSortState *state)
{
    if (state->i < ELEMENT_COUNT - 1)
    {
        if (state->j > 0)
        {
            if (arr[state->j] < arr[state->j - 1])
            {
                int tmp = arr[state->j - 1];
                arr[state->j - 1] = arr[state->j];
                arr[state->j] = tmp;
            }
            state->j -= 1;
        }
        else
        {
            state->i += 1;
            state->j = state->i + 1;
        }
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