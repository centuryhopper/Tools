#include "raylib.h"
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include "../include/utils.h"
#include "../include/configs.h"

#include <stdlib.h>

static int quickSortPartition(int *arr, int size, int lo, int hi)
{
  if (WindowShouldClose())
    return lo; // early exit if window closed

  if (hi <= 0)
    return lo;

  // pick a random pivotIdx between hi and lo
  int pivotIdx = lo + (rand() % (hi - lo));
  int tmpLo = arr[lo];
  arr[lo] = arr[pivotIdx];
  arr[pivotIdx] = tmpLo;
  // set pivotIdx to lo (our do while loop will make sure i and pivotIdx are not on the same index)
  pivotIdx = lo;
  int i = lo;
  int j = hi;
  int left, right;

  while (i < j)
  {
    do
    {
      if (WindowShouldClose())
        return lo;

      i += 1;
      left = (i < hi) ? arr[i] : INT_MAX;
      BeginDrawing();
      draw_state(arr, i, -1, pivotIdx);
      EndDrawing();
      WaitTime(0.05);
    } while (left <= arr[pivotIdx]);

    do
    {
      if (WindowShouldClose())
        return lo;

      j -= 1;
      right = (j > lo) ? arr[j] : INT_MIN;
      BeginDrawing();
      draw_state(arr, -1, j, pivotIdx);
      EndDrawing();
      WaitTime(0.05);
    } while (right > arr[pivotIdx]);

    if (i < j)
    {
      int tmp = arr[j];
      arr[j] = arr[i];
      arr[i] = tmp;
    }
  }

  int tmp = arr[j];
  arr[j] = arr[pivotIdx];
  arr[pivotIdx] = tmp;

  return j;
}

static void runner(int *arr, int size, int lo, int hi)
{
  if (WindowShouldClose())
    return; // check before continuing
  if (hi - lo <= 1)
  {
    return;
  }

  int partitionIdx = quickSortPartition(arr, size, lo, hi);
  runner(arr, size, lo, partitionIdx);
  runner(arr, size, partitionIdx + 1, hi);
}

void quickSort(int *arr)
{
  srand((unsigned int)time(NULL));
  runner(arr, ELEMENT_COUNT, 0, ELEMENT_COUNT);
  printf("quick sort is done\n");
}

static int quickSortPartitionRaw(int *arr, int size, int lo, int hi)
{
  int pivotIdx = lo + (rand() % (hi - lo));
  // printf("pivotIdx: %d\n", pivotIdx);

  // swap pivotIdx value with lo
  int tmpLo = arr[lo];
  arr[lo] = arr[pivotIdx];
  arr[pivotIdx] = tmpLo;
  // set pivotIdx to lo (our do while loop will make sure i and pivotIdx are not on the same index)
  pivotIdx = lo;
  int i = lo;
  int j = hi;

  int left, right;

  while (i < j)
  {
    do
    {
      i++;
      left = (i < hi) ? arr[i] : INT_MAX;
    } while (left <= arr[pivotIdx]);

    // printf("here\n");
    do
    {
      j--;
      right = (j > lo) ? arr[j] : INT_MIN;
    } while (right > arr[pivotIdx]);

    if (i < j)
    {
      arr[i] = right;
      arr[j] = left;
    }
  }

  int tmp = arr[pivotIdx];
  arr[pivotIdx] = arr[j];
  arr[j] = tmp;

  return j;
}

void quickSortRaw(int *arr, int size, int lo, int hi)
{
  if (hi - lo <= 1)
  {
    return;
  }
  int partitionIdx = quickSortPartitionRaw(arr, size, lo, hi);
  quickSortRaw(arr, size, lo, partitionIdx);
  quickSortRaw(arr, size, partitionIdx + 1, hi);
}
