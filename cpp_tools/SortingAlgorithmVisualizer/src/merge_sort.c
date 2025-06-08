#include "raylib.h"
#include "../include/configs.h"
#include "../include/utils.h"
#include "../include/merge_sort.h"
#include <stdlib.h>
#include <stdio.h>

void cleanUpMergeSortState(MergeSortState **state)
{
  if (state && *state)
  {
    free((*state)->tempArray);
    (*state)->tempArray = NULL;
    free(*state);
    *state = NULL;
  }
}

void initializeMergeSortState(MergeSortState **state)
{
  if (*state)
  {
    cleanUpMergeSortState(state);
  }

  *state = malloc(sizeof(MergeSortState));
  if (!(*state))
  {
    printf("merge sort malloc failed\n");
    return;
  }

  (*state)->subArrayWidth = 1;
  (*state)->section = 0;
  (*state)->tempArrayIdx = 0;
  (*state)->tempArray = malloc(sizeof(int) * ELEMENT_COUNT);
}

// Iterative merge sort implementation runtime: O(ceil(log (n))), where n is the number of elements to be sorted
void mergeSort(int *arr, MergeSortState *state)
{
  if (state->subArrayWidth < ELEMENT_COUNT)
  {
    if (state->section < ELEMENT_COUNT)
    {
      int left = state->section;
      int mid = state->section + state->subArrayWidth;
      int right = state->section + 2 * state->subArrayWidth;
      mid = MIN(mid, ELEMENT_COUNT);
      right = MIN(right, ELEMENT_COUNT);

      // Merge arr[left:mid] and arr[mid:right] into temp
      int l = left, r = mid, t = left;
      while (l < mid && r < right)
        state->tempArray[t++] = arr[l] < arr[r] ? arr[l++] : arr[r++];
      while (l < mid)
        state->tempArray[t++] = arr[l++];
      while (r < right)
        state->tempArray[t++] = arr[r++];

      state->section += (2 * state->subArrayWidth);
    }
    else
    {
      if (state->tempArrayIdx < ELEMENT_COUNT)
      {
        // copy a newly formed pass from the temp array over to arr
        arr[state->tempArrayIdx] = state->tempArray[state->tempArrayIdx];
        state->tempArrayIdx += 1;
      }
      else
      {
        state->section = 0;
        state->subArrayWidth *= 2;
        state->tempArrayIdx = 0;
      }
    }
  }

}

void mergeSortIterativeRaw(int *arr, int n)
{
  int *temp = (int *)malloc(n * sizeof(int));
  for (int width = 1; width < n; width *= 2)
  {
    for (int i = 0; i < n; i += 2 * width)
    {
      int left = i;
      int mid = i + width;
      int right = i + 2 * width;
      if (mid > n)
        mid = n;
      if (right > n)
        right = n;

      // Merge arr[left:mid] and arr[mid:right] into temp
      int l = left, r = mid, t = left;
      while (l < mid && r < right)
        temp[t++] = arr[l] < arr[r] ? arr[l++] : arr[r++];
      while (l < mid)
        temp[t++] = arr[l++];
      while (r < right)
        temp[t++] = arr[r++];
    }
    // Copy back to original array
    for (int i = 0; i < n; i++)
      arr[i] = temp[i];
  }
  free(temp);
}

static void mergeyMergeRaw(int *arr, int left, int mid, int right)
{
  int tmpArraySize = right - left + 1;
  int tmp[tmpArraySize];
  int i = left, j = mid + 1, k = 0;
  while (i <= mid && j <= right)
  {
    if (arr[i] <= arr[j])
      tmp[k++] = arr[i++];
    else
      tmp[k++] = arr[j++];
  }

  while (i <= mid)
    tmp[k++] = arr[i++];
  while (j <= right)
    tmp[k++] = arr[j++];

  // copy tmp array contents over to original v
  for (int p = 0; p < tmpArraySize; ++p)
  {
    arr[left + p] = tmp[p];
  }
}

void mergeSortRaw(int *arr, int left, int right)
{
  if (left >= right)
    return;

  // postorder style traversal
  int mid = left + (right - left) / 2;
  mergeSortRaw(arr, left, mid);
  mergeSortRaw(arr, mid + 1, right);
  mergeyMergeRaw(arr, left, mid, right);
}
