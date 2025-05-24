#ifndef SELECTION_SORT_H
#define SELECTION_SORT_H

typedef struct
{
    int i, j, minIdx;
    int swapped;
} SelectionSortState;

SelectionSortState *cleanUpSelectionSortState(SelectionSortState *state);
SelectionSortState *initializeSelectionSortState(SelectionSortState *state, SelectionSortState values);
void selectionSortRaw(int *arr, int size);
void selectionSort(int* arr, SelectionSortState* state);

#endif