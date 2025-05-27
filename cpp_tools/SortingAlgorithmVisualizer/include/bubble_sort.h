#ifndef BUBBLE_SORT_H
#define BUBBLE_SORT_H

typedef struct
{
    int i, j;
    int swapped;
    int sorting;
} BubbleSortState;

void cleanUpBubbleSortState(BubbleSortState **state);
void initializeBubbleSortState(BubbleSortState **state);
void bubbleSort(int *arr, BubbleSortState *state);
void bubbleSortRaw(int *arr, int size);

#endif