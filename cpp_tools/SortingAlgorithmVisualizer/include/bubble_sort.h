#ifndef BUBBLE_SORT_H
#define BUBBLE_SORT_H

typedef struct
{
    // State variables for each sort type
    int i, j;    // For bubble, insertion, selection
    int swapped; // bubble
                 // Add other vars for quicksort, merge sort if needed
    int sorting;
} BubbleSortState;

BubbleSortState* cleanUpBubbleSortState(BubbleSortState* state);
BubbleSortState* initializeBubbleSortState(BubbleSortState* state, BubbleSortState values);
void bubbleSort(int* arr, BubbleSortState *state);
void bubbleSortRaw(int *arr, int size);

#endif