#ifndef QUICK_SORT_H
#define QUICK_SORT_H

typedef struct
{
    int l, h;
    int top;
    int *stack;
    int pivot;
} QuickSortState;

// void cleanUpQuickSortState(QuickSortState **state);
// void initializeQuickSortState(QuickSortState **state);
void quickSortRaw(int *arr, int lo, int hi);
void quickSort(int *arr, QuickSortState* state);

#endif