#ifndef MERGE_SORT_H
#define MERGE_SORT_H


typedef struct
{
    int subArrayWidth;
    int section;
    int *tempArray;
    int tempArrayIdx;
    bool isSortDone;
} MergeSortState;

void cleanUpMergeSortState(MergeSortState **state);
void initializeMergeSortState(MergeSortState **state);
void mergeSortRaw(int *arr, int left, int right);
void mergeSortIterativeRaw(int *arr, int size);
void mergeSortIterative(int *arr, int size);
void mergeSort(int *arr, MergeSortState *state);

#endif

