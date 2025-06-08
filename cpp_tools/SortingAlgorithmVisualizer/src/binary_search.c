#include "raylib.h"
#include "../include/configs.h"
#include "../include/utils.h"
#include "../include/binary_search.h"
#include <stdlib.h>
#include <stdio.h>

void cleanUpBinarySearchState(BinarySearchState **state)
{
    if (*state)
    {
        free(*state);
        *state = NULL;
    }
}

void initializeBinarySearchState(BinarySearchState **state)
{
    // printf("state: %d\n", *state == NULL);
    if (*state)
    {
        cleanUpBinarySearchState(state);
    }
    *state = malloc(sizeof(BinarySearchState));
    if (!(*state))
    {
        printf("binary search malloc failed\n");
        return;
    }
    (*state)->lo = 0;
    (*state)->mid = (*state)->lo + ((*state)->hi - (*state)->lo) / 2;
    (*state)->answerIdx = -1;
    (*state)->isDone = 0;
    (*state)->hi = ELEMENT_COUNT - 1;
}

void binarySearch(int *arr, int target, BinarySearchState *state)
{
    // static int call = 0;
    // printf("called %d\n", ++call);
    state->mid = state->lo + (state->hi - state->lo) / 2;
    // printf("mid: %d\n", state->mid);
    if (state->lo <= state->hi)
    {
        
        if (arr[state->mid] > target)
        {
            // mid is too high, look left
            state->hi = state->mid - 1;
        }
        else if (arr[state->mid] < target)
        {
            // mid is too low, look right
            state->lo = state->mid + 1;
        }
        else
        {
            state->answerIdx = state->mid;
            state->isDone = 1;
        }
        // printf("searching procedure\n");
    }
    else
    {
        state->isDone = 1;
    }
}

int binarySearchRaw(int *arr, int numsSize, int target)
{
    int l = 0;
    int h = numsSize - 1;
    int mid = l + (h - l) / 2;
    while (l <= h)
    {
        if (arr[mid] > target)
        {
            // mid is too high, look left
            h = mid - 1;
        }
        else if (arr[mid] < target)
        {
            // mid is too low, look right
            l = mid + 1;
        }
        else
        {
            return mid;
        }

        mid = l + (h - l) / 2;
    }

    // value not found
    return -1;
}