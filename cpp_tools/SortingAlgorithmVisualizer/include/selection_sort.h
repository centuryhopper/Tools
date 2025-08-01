#ifndef SELECTION_SORT_H
#define SELECTION_SORT_H

typedef struct
{
    int i, j, minIdx;
    int swapped;
} SelectionSortState;

void selectionSortRaw(int *arr, int size);
void selectionSort(int* arr, SelectionSortState* state);

#endif