#include "raylib.h"
#include <limits.h>
#include <stdio.h>
#include <time.h>
#include "../include/utils.h"
#include "../include/configs.h"
#include "../include/quick_sort.h"

#include <stdlib.h>

static void quickSortIterative(int *arr, int l, int h);
static int quickSortPartitionRaw(int *arr, int lo, int hi);
static void iterativeRunner(int *arr, QuickSortState *state);

void cleanUpQuickSortState(QuickSortState **state)
{
  if (state && *state)
  {
    free((*state)->stack);
    (*state)->stack = NULL;
    free(*state);
    *state = NULL;
  }
}

void initializeQuickSortState(QuickSortState **state)
{
  if (*state)
  {
    cleanUpQuickSortState(state);
  }
  *state = malloc(sizeof(QuickSortState));
  if (!(*state))
  {
    printf("quick sort malloc failed\n");
    return;
  }

  (*state)->l = 0;
  (*state)->h = ELEMENT_COUNT;
  (*state)->top = -1;
  (*state)->stack = (int *)calloc((ELEMENT_COUNT + 1), sizeof(int));
  // Push initial values of l and h to stack
  (*state)->stack[++(*state)->top] = (*state)->l;
  (*state)->stack[++(*state)->top] = (*state)->h;
}

void quickSort(int *arr, QuickSortState *state)
{
  srand((unsigned int)time(NULL));
  iterativeRunner(arr, state);
}

static void iterativeRunner(int *arr, QuickSortState *state)
{
  // Keep popping while stack is not empty
  if (state->top >= 0)
  {
    // Pop h and l
    state->h = state->stack[state->top--];
    state->l = state->stack[state->top--];

    // Partition the array
    int p = quickSortPartitionRaw(arr, state->l, state->h);
    state->pivot = p;

    // exclude lo if same of greater than pivot index
    if (p > state->l)
    {
      state->stack[++state->top] = state->l;
      state->stack[++state->top] = p;
    }

    // If right side of pivot has more than 1 element
    if (p + 1 < state->h)
    {
      state->stack[++state->top] = p + 1;
      state->stack[++state->top] = state->h;
    }
  }
}

// Iterative quicksort using a stack
static void quickSortIterative(int *arr, int l, int h)
{
  // Stack to hold subarray start and end indices
  int stack[h - l + 1];

  // Initialize top of stack
  int top = -1;

  // Push initial values of l and h to stack
  stack[++top] = l;
  stack[++top] = h;

  // Keep popping while stack is not empty
  while (top >= 0)
  {
    // Pop h and l
    h = stack[top--];
    l = stack[top--];

    // Partition the array
    int p = quickSortPartitionRaw(arr, l, h);

    // exclude lo if same of greater than pivot index
    if (p > l)
    {
      stack[++top] = l;
      stack[++top] = p;
    }

    // If right side of pivot has more than 1 element
    if (p + 1 < h)
    {
      stack[++top] = p + 1;
      stack[++top] = h;
    }
  }
}

static int quickSortPartitionRaw(int *arr, int lo, int hi)
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

void quickSortRaw(int *arr, int lo, int hi)
{
  if (hi - lo <= 1)
  {
    return;
  }
  int partitionIdx = quickSortPartitionRaw(arr, lo, hi);
  quickSortRaw(arr, lo, partitionIdx);
  quickSortRaw(arr, partitionIdx + 1, hi);
}
