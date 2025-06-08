#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H

typedef struct
{
    int lo, mid, hi;
    int answerIdx;
    int isDone;
} BinarySearchState;

void cleanUpBinarySearchState(BinarySearchState **state);
void initializeBinarySearchState(BinarySearchState **state);
void binarySearch(int *arr, int target, BinarySearchState *state);
int binarySearchRaw(int *arr, int numsSize, int target);

#endif