#include "raylib.h"
#include "../include/configs.h"
#include "../include/draw_state.h"
#include "../include/selection_sort.h"
#include <stdlib.h>

SelectionSortState *cleanUpSelectionSortState(SelectionSortState *state)
{
    if (state)
    {
        free(state);
    }
    return NULL;
}

SelectionSortState *initializeSelectionSortState(SelectionSortState *state, SelectionSortState values)
{
    if (state)
    {
        state = cleanUpSelectionSortState(state);
    }
    state = malloc(sizeof(SelectionSortState));
    if (!state)
    {
        return NULL;
    }
    state->i = values.i;
    state->j = values.j;
    state->minIdx = values.minIdx;
    state->swapped = values.swapped;
    return state;
}

// Selection Sort with per-frame visualization
void selectionSort(int *arr, SelectionSortState *state)
{
    // assume ith index is the min so far
    if (state->i != ELEMENT_COUNT)
    {
        if (state->j < ELEMENT_COUNT)
        {
            if (arr[state->minIdx] > arr[state->j])
            {
                state->minIdx = state->j;
                state->swapped = 1;
            }
            draw_state(arr, state->i, state->j, -1);
            state->j += 1;
        }
        else
        {
            if (state->swapped)
            {
                int tmp = arr[state->i];
                arr[state->i] = arr[state->minIdx];
                arr[state->minIdx] = tmp;
            }
            state->swapped = 0;
            state->i+=1;
            state->minIdx=state->i;
            state->j = state->i+1;
        }
    }
    
}

void selectionSortRaw(int *arr, int size)
{
    if (size < 2)
        return;
    for (int i = 0; i < size; i++)
    {
        // assume ith index is the min so far
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
}