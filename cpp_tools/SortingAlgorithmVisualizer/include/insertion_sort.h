#ifndef INSERTION_SORT_H
#define INSERTION_SORT_H
typedef struct
{
    int i, j;
} InsertionSortState;

void cleanUpInsertionSortState(InsertionSortState** state);
void initializeInsertionSortState(InsertionSortState** state);
void insertionSortRaw(int *arr, int size);
void insertionSort(int *arr, InsertionSortState *state);

#endif


