#include "raylib.h"
#include "../include/configs.h"
#include "../include/draw_state.h"
#include <stdlib.h>
#include <stdio.h>

// Merge function with visualization
static void mergeyMerge(int *arr, int left, int mid, int right)
{
  int tmpArraySize = right - left + 1;
  int tmp[tmpArraySize];
  int i = left, j = mid + 1, k = 0;

  while (i <= mid && j <= right)
  {
    if (WindowShouldClose())
      return;

    if (arr[i] <= arr[j])
      tmp[k++] = arr[i++];
    else
      tmp[k++] = arr[j++];

    BeginDrawing();
    draw_state(arr, i, j, -1);
    EndDrawing();
    WaitTime(WAIT_TIME);
  }

  while (i <= mid)
  {
    if (WindowShouldClose())
      return;
    tmp[k++] = arr[i++];
  }

  while (j <= right)
  {
    if (WindowShouldClose())
      return;
    tmp[k++] = arr[j++];
  }

  for (int p = 0; p < tmpArraySize; ++p)
  {
    if (WindowShouldClose())
      return;

    arr[left + p] = tmp[p];

    BeginDrawing();
    draw_state(arr, left + p, -1, -1);
    EndDrawing();
    WaitTime(WAIT_TIME);
  }
}

// Recursive runner with exit check
void runner(int *arr, int left, int right)
{
  if (WindowShouldClose())
    return;
  if (left >= right)
    return;

  int mid = left + (right - left) / 2;

  runner(arr, left, mid);
  if (WindowShouldClose())
    return;

  runner(arr, mid + 1, right);
  if (WindowShouldClose())
    return;

  mergeyMerge(arr, left, mid, right);
}

// Public interface
void mergeSort(int *arr)
{
  runner(arr, 0, ELEMENT_COUNT);

  // printf("merge sort done\n");
}

void mergeSortIterative(int *arr, int n)
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
    {
      BeginDrawing();
      EndDrawing();
      WaitTime(WAIT_TIME);
      arr[i] = temp[i];
      draw_state(arr, i, -1, -1);
    }
  }
  free(temp);
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
