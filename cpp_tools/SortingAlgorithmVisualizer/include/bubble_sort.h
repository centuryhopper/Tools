#ifndef BUBBLE_SORT_H
#define BUBBLE_SORT_H

typedef struct
{
    int i, j;
    int swapped;
    int sorting;
} BubbleSortState;

BubbleSortState* cleanUpBubbleSortState(BubbleSortState* state);
BubbleSortState* initializeBubbleSortState(BubbleSortState* state, BubbleSortState values);
void bubbleSort(int* arr, BubbleSortState *state);
void bubbleSortRaw(int *arr, int size);

#endif