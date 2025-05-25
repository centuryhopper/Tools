#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
#include "../include/configs.h"
#include "../include/draw_state.h"
#include "../include/bubble_sort.h"
#include "../include/insertion_sort.h"
#include "../include/merge_sort.h"
#include "../include/quick_sort.h"
#include "../include/selection_sort.h"
#include "../include/utils.h"
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WIDGET_WIDTH 400
#define WIDGET_HEIGHT 100
#define SPACING 75

static void initializeArray(int *arr);

/*
    raygui:
      wget https://raw.githubusercontent.com/raysan5/raygui/master/src/raygui.h -O raygui.h

    compile to wasm pre-reqs:
        cd raylib/src
        emmake make PLATFORM=PLATFORM_WEB
    compile to wasm:
        source ~/emsdk/emsdk_env.fish && make web

    start server:
        python3 -m http.server
        then go to http://localhost:8000/wasm/main.html
*/
// "Bubble;Insertion;Selection;Quick;Merge"
typedef enum
{
  BUBBLE = 0,
  INSERTION = 1,
  SELECTION = 2,
  QUICK = 3,
  MERGE = 4,
  NONE = -1,
} SortType;

const char *getSortTypeString(SortType sortType)
{
  switch (sortType)
  {
  case SELECTION:
    return "SELECTION";
  case INSERTION:
    return "INSERTION";
  case BUBBLE:
    return "BUBBLE";
  case QUICK:
    return "QUICK";
  case MERGE:
    return "MERGE";
  default:
    return "NONE";
  }
}

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
    printf("%d%s", data[i], (i == ELEMENT_COUNT - 1) ? "\n" : " ");
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

// contains the actual sort logic
static void runSort(SortType type, int *data, void *sortState)
{
  // TODO: complete the other sorts
  switch (type)
  {
  case SELECTION:
    selectionSort(data, (SelectionSortState *)sortState);
    break;
  case INSERTION:
    insertionSort(data, (InsertionSortState *)sortState);
    break;
  case BUBBLE:
    bubbleSort(data, (BubbleSortState *)sortState);
    break;
  case QUICK:
    quickSort(data);
    break;
  case MERGE:
    mergeSort(data);
    break;
  default:
    break;
  }
}

static void resetSortState(SortType type, int *data, void **sortState)
{
  // TODO: complete the other sorts
  switch (type)
  {
  case SELECTION:
    *sortState = (SelectionSortState *)initializeSelectionSortState((SelectionSortState *)*sortState, (SelectionSortState){
                                                                                                          .i = 0,
                                                                                                          .j = 1,
                                                                                                          .minIdx = 0,
                                                                                                          .swapped = 0,
                                                                                                      });
    break;
  case INSERTION:
    *sortState = (InsertionSortState *)initializeInsertionSortState((InsertionSortState *)*sortState, (InsertionSortState){
                                                                                                          .i = 0,
                                                                                                          .j = 1,
                                                                                                      });
    break;
  case BUBBLE:
    *sortState = (BubbleSortState *)initializeBubbleSortState((BubbleSortState *)*sortState, (BubbleSortState){.i = ELEMENT_COUNT - 1, .j = 1, .swapped = 0, .sorting = 1});
    break;
  case QUICK:
    break;
  case MERGE:
    break;
  default:
    break;
  }
}

static void cleanUpSortState(SortType type, void **sortState)
{
  // TODO: complete the other sorts
  switch (type)
  {
  case SELECTION:
    *sortState = cleanUpSelectionSortState((SelectionSortState *)*sortState);
    break;
  case INSERTION:
    *sortState = cleanUpInsertionSortState((InsertionSortState *)*sortState);
    break;
  case BUBBLE:
    *sortState = cleanUpBubbleSortState((BubbleSortState *)*sortState);
    break;
  case QUICK:
    break;
  case MERGE:
    break;
  default:
    break;
  }
}

static int visualizationTest(int *data)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sort Visualizer - raylib");
  SetTargetFPS(FPS);
  // Set raygui font size
  GuiSetStyle(DEFAULT, TEXT_SIZE, 50);

  int totalWidth = 4 * WIDGET_WIDTH + 2 * SPACING;
  int startX = (SCREEN_WIDTH - totalWidth) / 2;
  int y = 50; // vertical position
  int currentSortChoice = 0;
  int sortDropdownActive = 0;
  float speed = 0.5f;
  bool started = false;
  SortType sortChosen = NONE;

  initializeArray(data);

  const char *options = "Bubble;Insertion;Selection;Quick;Merge";

  void *sortState = NULL;
  double lastStepTime = GetTime();

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    if (!started)
    {
      // make sure to draw bars first to avoid appearing in front of the text ui
      draw_state_with_color(data, -1, -1, -1, BLUE);

      // dropdown
      Rectangle dropdownBounds = {startX - 100, y, WIDGET_WIDTH, WIDGET_HEIGHT};
      if (GuiDropdownBox(dropdownBounds, options, &currentSortChoice, sortDropdownActive))
      {
        sortDropdownActive = !sortDropdownActive;
      }

      Rectangle sliderBounds = {startX + WIDGET_WIDTH + SPACING, y, WIDGET_WIDTH, WIDGET_HEIGHT};
      GuiSliderBar(sliderBounds, "Speed", NULL, &speed, 0.001f, 0.999f);

      // printf("speed: %f\n", speed);

      Rectangle buttonBounds = {startX + 2 * (WIDGET_WIDTH + SPACING), y, WIDGET_WIDTH, WIDGET_HEIGHT};
      if (GuiButton(buttonBounds, "Start"))
      {
        started = true;
        sortChosen = (SortType)currentSortChoice;
        resetSortState(sortChosen, data, &sortState);
        // printf("i initted: %d\n", ((BubbleSortState *)(sortState))->i);
        lastStepTime = GetTime(); // Reset timing
      }
    }
    else
    {
      // sort logic goes here
      // IMPORTANT: We don't use WaitTime because WaitTime will block
      double currentTime = GetTime();
      if (!isSorted(data, ELEMENT_COUNT) && (currentTime - lastStepTime >= (1.0 - speed)))
      {
        runSort(sortChosen, data, sortState);
        lastStepTime = currentTime;
      }

      int i, j;
      switch (sortChosen)
      {
      case SELECTION:;
        SelectionSortState *sState = (SelectionSortState *)sortState;
        i = sState->i;
        j = sState->j;
        // printf("i: %d, j: %d\n", i, j);
        draw_state(data, i, j, -1);
        // printf("Selection!\n");
        break;
      case BUBBLE:;
        BubbleSortState *bState = (BubbleSortState *)sortState;
        i = bState->i;
        j = bState->j;
        draw_state(data, i, j, -1);
        break;
      case INSERTION:;
        InsertionSortState *iState = (InsertionSortState *)sortState;
        i = iState->i;
        j = iState->j;
        draw_state(data, i, j, -1);
        break;
      case MERGE:
        break;
      case QUICK:
        break;
      default:
        break;
      }

      // runSort(sortChosen, data, sortState);

      // draw_state_with_color(data, -1, -1, -1, started ? YELLOW : BLUE);
    }

    if (started && isSorted(data, ELEMENT_COUNT))
    {
      DrawText("Sorted!", SCREEN_WIDTH / 2 - 100, 50, 40, YELLOW);

      // Visualization bars
      draw_state_with_color(data, -1, -1, -1, started ? YELLOW : BLUE);
    }
    else if (started && !isSorted(data, ELEMENT_COUNT))
    {
      // printf("drawing it\n");
      DrawText(getSortTypeString(sortChosen), SCREEN_WIDTH / 2 - 100, 50, 40, YELLOW);
      // Visualization bars
      // draw_state_with_color(data, -1, -1, -1, started ? YELLOW : BLUE);
    }

    // Reset button
    Rectangle resetBounds = {startX + 3 * (WIDGET_WIDTH + SPACING), y, WIDGET_WIDTH, WIDGET_HEIGHT};
    if (GuiButton(resetBounds, started ? "Reset" : "Randomize"))
    {
      started = false;
      // Reset your data array here, for example:
      initializeArray(data);
      resetSortState(sortChosen, data, &sortState);
    }

    EndDrawing();
  }

  // clean up sort state here
  // printf("%s\n", !sortState ? "sort state cleaned up already? Something is wrong!" : "sort state not cleaned up but will be!");
  cleanUpSortState(sortChosen, &sortState);
  // printf("%s\n", !sortState ? "sort state cleaned up!" : "sort state not cleaned up! Okay something is definitely wrong!");
  CloseWindow();

  return EXIT_SUCCESS;
}

// initializes array with randomized values between 1 and 100
static void initializeArray(int *arr)
{
  for (int i = 0; i < ELEMENT_COUNT; ++i)
  {
    // just to show that quick sort runs in quadratic run time with this kind of
    // input
    // arr[i] = 100 - i;
    arr[i] = 1 + rand() % 100;
    // printf("%d%s", arr[i], (i == ELEMENT_COUNT - 1) ? "\n" : " ");
  }
}

int main(void)
{
  // printf("Number of arguments: %d\n", argc);
  // for (int i = 0; i < argc; i++) {
  //     printf("Argument %d: %s\n", i, argv[i]);
  // }

  // if (argc < 2)
  // {
  //   fprintf(stderr, "Error: Missing argument. Usage: %s <type of sort>\n",
  //           argv[0]);
  //   return EXIT_FAILURE; // Non-zero to indicate error
  // }

  // printf("raw test? (y/n)\n");
  // char input;
  // scanf(" %c", &input); // ✅ Recommended for reading single characters
  // input = tolower(input);

  int data[ELEMENT_COUNT];
  srand((unsigned int)time(NULL));

  visualizationTest(data);

  // printf("===========================================\n");

  // switch (input)
  // {
  // case 'y':
  //   initializeArray(data);
  //   rawTest(argv[1], data);
  //   break;
  // case 'n':
  //   visualizationTest(argv[1], data);
  //   break;
  // default:
  //   fprintf(stderr, "Error: Invalid input\n");
  //   break;
  // }

  return 0;
}
