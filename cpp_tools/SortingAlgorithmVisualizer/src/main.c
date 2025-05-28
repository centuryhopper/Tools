#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"
#include "../include/configs.h"
#include "../include/utils.h"
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

// "Bubble;Insertion;Selection;Quick;Merge"
typedef enum
{
  QUICK = 0,
  MERGE = 1,
  BUBBLE = 2,
  INSERTION = 3,
  SELECTION = 4,
  NONE = -1,
} SortType;

const char *options = "Quick;Merge;Bubble;Insertion;Selection;";
static SortType prevChosenSortType = NONE;
static void cleanUpSortState(SortType type, void **sortState);

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

const char *getSortTypeString(const SortType sortType)
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

static void visualizeBars(SortType sortChosen, int *data, void *sortState)
{
  if (!sortState)
  {
    return;
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
  case MERGE:;
    MergeSortState *mState = (MergeSortState *)sortState;
    i = mState->tempArrayIdx;
    draw_state(data, i, -1, -1);
    break;
  case QUICK:;
    QuickSortState *qState = (QuickSortState *)sortState;
    int pivot = qState->pivot;
    int lo = qState->l;
    int hi = qState->h;
    // printf("pivot: %d\n", pivot);
    draw_state(data, pivot, lo, hi);
    break;
  default:
    break;
  }
}

static int rawTest(char *arg, int *data)
{
  initializeArray(data);
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
    // printf("quick sorting\n");
    quickSortRaw(data, 0, ELEMENT_COUNT);
    // quickSort(data);
    // printf("quick sorting done\n");
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

  printArray(data);

  if (isSorted(data))
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
  case QUICK:;
    QuickSortState *qSortState = (QuickSortState *)sortState;
    quickSort(data, qSortState);
    break;
  case MERGE:;
    MergeSortState *mSortState = (MergeSortState *)sortState;
    mergeSort(data, (MergeSortState *)sortState);
    break;
  default:
    break;
  }
}

static void resetSortState(SortType type, int *data, void **sortState)
{
  if (*sortState)
  {
    // printf("%s\n", getSortTypeString(prevChosenSortType));
    cleanUpSortState(prevChosenSortType, sortState);
    // printf("cleaned up %s\n", getSortTypeString(prevChosenSortType));
  }
  prevChosenSortType = type;
  switch (type)
  {
  case SELECTION:
    initializeSelectionSortState((SelectionSortState **)sortState);
    break;
  case INSERTION:
    initializeInsertionSortState((InsertionSortState **)sortState);
    break;
  case BUBBLE:
    initializeBubbleSortState((BubbleSortState **)sortState);
    break;
  case QUICK:
    initializeQuickSortState((QuickSortState **)sortState);
    break;
  case MERGE:
    // printf("reseting merge sort\n");
    initializeMergeSortState((MergeSortState **)sortState);
    // printf("done reseting merge sort\n");
    break;
  default:
    break;
  }
}

static void cleanUpSortState(SortType type, void **sortState)
{
  if (!(*sortState))
  {
    return;
  }
  switch (type)
  {
  case SELECTION:
    cleanUpSelectionSortState((SelectionSortState **)sortState);
    break;
  case INSERTION:
    cleanUpInsertionSortState((InsertionSortState **)sortState);
    break;
  case BUBBLE:
    cleanUpBubbleSortState((BubbleSortState **)sortState);
    break;
  case QUICK:
    cleanUpQuickSortState((QuickSortState **)sortState);
    break;
  case MERGE:
    cleanUpMergeSortState((MergeSortState **)sortState);
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

      Rectangle sliderBounds = {startX - 100, y, WIDGET_WIDTH, WIDGET_HEIGHT};
      GuiSliderBar(sliderBounds, "Speed", NULL, &speed, 0.001f, 0.999f);

      // printf("speed: %f\n", speed);

      // dropdown
      Rectangle dropdownBounds = {startX + WIDGET_WIDTH + SPACING, y, WIDGET_WIDTH, WIDGET_HEIGHT};
      if (GuiDropdownBox(dropdownBounds, options, &currentSortChoice, sortDropdownActive))
      {
        sortDropdownActive = !sortDropdownActive;
      }

      Rectangle buttonBounds = {startX + 2 * (WIDGET_WIDTH + SPACING), y, WIDGET_WIDTH, WIDGET_HEIGHT};
      if (GuiButton(buttonBounds, "Start"))
      {
        started = true;
        sortChosen = (SortType)currentSortChoice;
        resetSortState(sortChosen, data, &sortState);

        lastStepTime = GetTime(); // Reset timing
      }
    }
    else
    {
      // sort logic goes here
      // IMPORTANT: We don't use WaitTime because WaitTime will block
      double currentTime = GetTime();
      if (!isSorted(data) && (currentTime - lastStepTime >= (1.0 - speed)))
      {
        runSort(sortChosen, data, sortState);
        lastStepTime = currentTime;
      }

      visualizeBars(sortChosen, data, sortState);

      // draw_state_with_color(data, -1, -1, -1, started ? YELLOW : BLUE);
    }

    if (started && isSorted(data))
    {
      DrawText("Sorted!", SCREEN_WIDTH / 2 - 100, 50, 40, YELLOW);

      cleanUpSortState(sortChosen, &sortState);

      // Visualization bars
      draw_state_with_color(data, -1, -1, -1, started ? YELLOW : BLUE);
    }
    else if (started && !isSorted(data))
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
  printf("%s\n", !sortState ? "sort state cleaned up already? Something is wrong!" : "sort state not cleaned up but will be!");
  cleanUpSortState(sortChosen, &sortState);
  printf("%s\n", !sortState ? "sort state cleaned up!" : "sort state not cleaned up! Okay something is definitely wrong!");
  CloseWindow();

  return EXIT_SUCCESS;
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

  // MUST have this seed before using initializeArray() calls
  srand((unsigned int)time(NULL));
  int data[ELEMENT_COUNT];

  visualizationTest(data);

  // make sure the string is writeable
  // char arg[] = "q";
  // rawTest(arg, data);

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
