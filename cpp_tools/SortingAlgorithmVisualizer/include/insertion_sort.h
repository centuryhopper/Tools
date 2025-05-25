#ifndef INSERTION_SORT_H
#define INSERTION_SORT_H
typedef struct
{
    int i, j;
} InsertionSortState;


InsertionSortState* cleanUpInsertionSortState(InsertionSortState* state);
InsertionSortState* initializeInsertionSortState(InsertionSortState* state, InsertionSortState values);
void insertionSortRaw(int *arr, int size);
void insertionSort(int *arr, InsertionSortState *state);

#endif


