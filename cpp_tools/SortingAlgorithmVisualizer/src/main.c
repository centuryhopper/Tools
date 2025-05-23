#include "../include/configs.h"
#include "../include/draw_state.h"
#include "../include/bubble_sort.h"
#include "../include/insertion_sort.h"
#include "../include/merge_sort.h"
#include "../include/quick_sort.h"
#include "../include/selection_sort.h"
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
    compile to wasm pre-reqs:
        cd raylib/src
        emmake make PLATFORM=PLATFORM_WEB
    compile to wasm:
        source ~/emsdk/emsdk_env.fish && make web

    start server:
        python3 -m http.server
        then go to http://localhost:8000/wasm/main.html
*/

static void str_to_lower(char *str)
{
  while (*str)
  {
    *str = tolower((unsigned char)*str);
    str++;
  }
}

static bool isSorted(const int *arr, int n)
{
  for (int i = 1; i < n; ++i)
  {
    if (arr[i] < arr[i - 1])
    {
      return false; // Found a pair out of order
    }
  }
  return true; // All pairs in order
}

static int rawTest(char *arg, int *data)
{
  str_to_lower(arg);

  if (strcmp(arg, "s") == 0)
  {
    selectionSortRaw(data, ELEMENT_COUNT);
  }
  else if (strcmp(arg, "i") == 0)
  {
    insertionSortRaw(data, ELEMENT_COUNT);
  }
  else if (strcmp(arg, "b") == 0)
  {
    bubbleSortRaw(data, ELEMENT_COUNT);
  }
  else if (strcmp(arg, "q") == 0)
  {
    quickSortRaw(data, ELEMENT_COUNT, 0, ELEMENT_COUNT);
  }
  else if (strcmp(arg, "m") == 0)
  {
    mergeSortRaw(data, 0, ELEMENT_COUNT);
  }
  else
  {
    fprintf(stderr, "Error: Unknown argument. Please choose one of the "
                    "following: (s,i,b,q,m)\n");
    return EXIT_FAILURE; // Non-zero to indicate error
  }

  for (int i = 0; i < ELEMENT_COUNT; i++)
  {
    printf("%d%s", data[i], (i == ELEMENT_COUNT-1) ? "\n" : " ");
  }

  if (isSorted(data, ELEMENT_COUNT))
  {
    printf("sorted\n");
  }
  else
  {
    printf("not sorted\n");
  }

  return EXIT_SUCCESS;
}

static int visualizationTest(char *arg, int *data)
{
  str_to_lower(arg);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sort Visualizer - raylib");
  SetTargetFPS(60);

  if (strcmp(arg, "s") == 0)
  {
    selectionSort(data);
  }
  else if (strcmp(arg, "i") == 0)
  {
    insertionSort(data);
  }
  else if (strcmp(arg, "b") == 0)
  {
    bubbleSort(data);
  }
  else if (strcmp(arg, "q") == 0)
  {
    quickSort(data);
  }
  else if (strcmp(arg, "m") == 0)
  {
    mergeSort(data);
  }
  else
  {
    fprintf(stderr, "Error: Unknown argument. Please choose one of the "
                    "following: (s,i,b,q,m)\n");
    return EXIT_FAILURE; // Non-zero to indicate error
  }

  for (int i = 0; i < ELEMENT_COUNT; i++)
  {
    printf("%d%s", data[i], (i == ELEMENT_COUNT-1) ? "\n" : " ");
  }

  if (isSorted(data, ELEMENT_COUNT))
  {
    printf("sorted\n");
  }
  else
  {
    printf("not sorted\n");
  }

  while (!WindowShouldClose())
  {
    BeginDrawing();
    draw_state(data, -1, -1, -1); // Final state
    EndDrawing();
  }

  CloseWindow();

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  // printf("Number of arguments: %d\n", argc);
  // for (int i = 0; i < argc; i++) {
  //     printf("Argument %d: %s\n", i, argv[i]);
  // }

  if (argc < 2)
  {
    fprintf(stderr, "Error: Missing argument. Usage: %s <type of sort>\n",
            argv[0]);
    return EXIT_FAILURE; // Non-zero to indicate error
  }

  printf("raw test? (y/n)\n");
  char input;
  scanf(" %c", &input); // ✅ Recommended for reading single characters
  input = tolower(input);

  int data[ELEMENT_COUNT];
  srand((unsigned int) time(NULL));
  for (int i = 0; i < ELEMENT_COUNT; ++i)
  {
    // just to show that quick sort runs in quadratic run time with this kind of
    // input
    // data[i] = 100 - i;
    data[i] = 1 + rand() % ELEMENT_COUNT;
    printf("%d%s", data[i], (i == ELEMENT_COUNT - 1) ? "\n" : " ");
  }

  printf("===========================================\n");

  switch (input)
  {
  case 'y':
    rawTest(argv[1], data);
    break;
  case 'n':
    visualizationTest(argv[1], data);
    break;
  default:
    fprintf(stderr, "Error: Invalid input\n");
    break;
  }

  return 0;
}
