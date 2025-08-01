#include "../include/utils.h"
#include "../include/selection_sort.h"
#include "../include/insertion_sort.h"
#include "../include/bubble_sort.h"
#include "../include/quick_sort.h"
#include "../include/merge_sort.h"
#include "../include/configs.h"
#include <stdio.h>
#include <stdlib.h>

void initState(SortType type, void **sortState);
void cleanUpState(SortType type, void **sortState);

void initState(SortType type, void **sortState)
{
    if (sortState == NULL)
    {
        return;
    }

    switch (type)
    {
    case INSERTION:;
        InsertionSortState **iState = (InsertionSortState **)sortState;
        if (*iState)
        {
            cleanUpState(INSERTION, sortState);
        }
        *iState = malloc(sizeof(InsertionSortState));
        if (!(*iState))
        {
            printf("insertion sort malloc failed\n");
            return;
        }
        (*iState)->i = 0;
        (*iState)->j = 1;
        break;
    case SELECTION:;
        SelectionSortState **selState = (SelectionSortState **)sortState;
        if (*selState)
        {
            cleanUpState(SELECTION, sortState);
        }
        *selState = malloc(sizeof(SelectionSortState));
        if (!(*selState))
        {
            printf("selection sort malloc failed\n");
            return;
        }
        (*selState)->i = 0;
        (*selState)->j = 1;
        (*selState)->minIdx = 0;
        (*selState)->swapped = 0;
        break;
    case BUBBLE:;
        BubbleSortState **bState = (BubbleSortState **)sortState;
        if (*bState)
        {
            cleanUpState(BUBBLE, sortState);
        }
        *bState = malloc(sizeof(BubbleSortState));
        if (!(*bState))
        {
            printf("bubble sort malloc failed\n");
            return;
        }
        (*bState)->i = ELEMENT_COUNT - 1;
        (*bState)->j = 1;
        (*bState)->swapped = 0;
        break;

    case QUICK:;
        QuickSortState **qState = (QuickSortState **)sortState;
        if (*qState)
        {
            cleanUpState(QUICK, sortState);
        }
        *qState = malloc(sizeof(QuickSortState));
        if (!(*qState))
        {
            printf("quick sort malloc failed\n");
            return;
        }

        (*qState)->l = 0;
        (*qState)->h = ELEMENT_COUNT;
        (*qState)->top = -1;
        (*qState)->stack = (int *)calloc((ELEMENT_COUNT + 1), sizeof(int));
        // Push initial values of l and h to stack
        (*qState)->stack[++(*qState)->top] = (*qState)->l;
        (*qState)->stack[++(*qState)->top] = (*qState)->h;
        break;
    case MERGE:;
        MergeSortState **mState = (MergeSortState **)sortState;
        if (*mState)
        {
            cleanUpState(MERGE, sortState);
        }

        *mState = malloc(sizeof(MergeSortState));
        if (!(*mState))
        {
            printf("merge sort malloc failed\n");
            return;
        }
        (*mState)->subArrayWidth = 1;
        (*mState)->section = 0;
        (*mState)->tempArrayIdx = 0;
        (*mState)->tempArray = malloc(sizeof(int) * ELEMENT_COUNT);
        break;
    default:
        printf("undefined sort type\n");
        break;
    }
}

void cleanUpState(SortType type, void **sortState)
{
    if (!(*sortState))
    {
        return;
    }
    switch (type)
    {
    case INSERTION:;
        InsertionSortState **iState = (InsertionSortState **)sortState;
        if (*iState)
        {
            free(*iState);
            *iState = NULL;
        }
        break;
    case SELECTION:;
        SelectionSortState **selState = (SelectionSortState **)sortState;
        if (*selState)
        {
            free((*selState));
            *selState = NULL;
        }
        break;
    case BUBBLE:;
        BubbleSortState **bState = (BubbleSortState **)sortState;
        if (*bState)
        {
            free(*bState);
            *bState = NULL;
        }
        break;

    case QUICK:;
        QuickSortState **qState = (QuickSortState **)sortState;
        if (qState && *qState)
        {
            free((*qState)->stack);
            (*qState)->stack = NULL;
            free(*qState);
            *qState = NULL;
        }
        break;
    case MERGE:;
        MergeSortState **mState = (MergeSortState **)sortState;
        if (mState && *mState)
        {
            free((*mState)->tempArray);
            (*mState)->tempArray = NULL;
            free(*mState);
            *mState = NULL;
        }
        break;
    default:
        printf("undefined sort type\n");
        break;
    }
}