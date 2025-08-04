#define RAYGUI_IMPLEMENTATION
#include "../include/binary_search.h"
#include "../include/bubble_sort.h"
#include "../include/configs.h"
#include "../include/insertion_sort.h"
#include "../include/merge_sort.h"
#include "../include/quick_sort.h"
#include "../include/raygui.h"
#include "../include/resource_manager.h"
#include "../include/selection_sort.h"
#include "../include/utils.h"
#include <ctype.h>
#include <limits.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
    raygui:
      wget https://raw.githubusercontent.com/raysan5/raygui/master/src/raygui.h
   -O raygui.h

    compile to wasm pre-reqs:
        cd raylib/src
        emmake make PLATFORM=PLATFORM_WEB
    compile to wasm:
        source ~/emsdk/emsdk_env.fish && make web

    start server:
        python3 -m http.server
        then go to http://localhost:8000/wasm/main.html
*/

#define WIDGET_WIDTH 400
#define WIDGET_HEIGHT 100
#define SPACING 75

const char *options = "Quick;Merge;Bubble;Insertion;Selection;";
const int totalWidth = 4 * WIDGET_WIDTH + 2 * SPACING;
const int y = 50; // vertical position
static int startX = (SCREEN_WIDTH - totalWidth) / 2;
static SortType prevChosenSortType = NONE;
static int toastTimer = 0;
static const int toastDuration = 120;
static bool showToast = false;
static char toastMessage[128];
static char inputBuffer[3];
bool shouldBinarySearch = false;

void ShowToast(char *message)
{
  // printf("message: %s\n", message);
  strncpy(toastMessage, message, sizeof(toastMessage) - 1);
  toastMessage[sizeof(toastMessage) - 1] = '\0'; // null-terminate
  toastTimer = toastDuration;
  showToast = true;
}

void DrawToast()
{
  // printf("showToast: %d\n", showToast);
  if (!showToast)
    return;

  int fontSize = 40;
  int padding = 40;
  int toastHeight = 100;

  // Draw toast background (centered at bottom)
  int toastWidth = MeasureText(toastMessage, fontSize) + padding * 2;
  int x = (GetScreenWidth() - toastWidth) / 2;
  int y = GetScreenHeight() - toastHeight - 40;

  // Draw toast background and text
  DrawRectangleRounded((Rectangle){x, y, toastWidth, toastHeight}, 0.2f, 10,
                       Fade(DARKGRAY, 0.85f));
  DrawText(toastMessage, x + padding, y + (toastHeight - fontSize) / 2,
           fontSize, RAYWHITE);

  toastTimer--;
  // printf("toast timer: %d\n", toastTimer);
  if (toastTimer <= 0)
  {
    showToast = false;
  }
}

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
    quickSortRaw(data, 0, ELEMENT_COUNT);
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
    quickSort(data, (QuickSortState *)sortState);
    break;
  case MERGE:;
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
    cleanUpState(prevChosenSortType, sortState);
    // printf("cleaned up %s\n", getSortTypeString(prevChosenSortType));
  }
  prevChosenSortType = type;
  // printf("type: %d\n", type);
  initState(type, sortState);
}

// Add text box for letting user binary search for a number in the sorted list
static void binarySearchUI(int *arr, double *bsTime,
                           BinarySearchState **state)
{
  bool hasOnlyDigits = true;
  // shouldBinarySearch = false;
  inputBuffer[2] = '\0';
  static bool textBoxEditMode = false;

  // Text box
  if (!shouldBinarySearch)
  {
    DrawText("Type a number between 1 and 100 below.", SCREEN_WIDTH / 2 - 300,
             150, 40, YELLOW);

    if (GuiTextBox((Rectangle){SCREEN_WIDTH / 2 - 200, 250, WIDGET_WIDTH,
                               WIDGET_HEIGHT},
                   inputBuffer, sizeof(inputBuffer), textBoxEditMode))
    {
      textBoxEditMode = !textBoxEditMode; // toggles editing mode when clicked
    }
  }

  // only allow ints inside the input text
  for (char *reader = inputBuffer; *reader != '\0'; reader++)
  {
    // printf("%c\n", *reader);
    if (!isdigit(*reader))
    {
      hasOnlyDigits = false;
    }
  }

  // printf("hasOnlyDigits: %d\n",hasOnlyDigits);

  if (!hasOnlyDigits)
  {
    if (!showToast)
    {
      printArray(arr);
      ShowToast("Please only enter numbers!");
    }
  }
  else if (hasOnlyDigits && inputBuffer[0] != '\0')
  {
    // Draw Submit Button
    if (GuiButton((Rectangle){SCREEN_WIDTH / 2 - 200, 400, WIDGET_WIDTH,
                              WIDGET_HEIGHT},
                  "Binary Search"))
    {
      shouldBinarySearch = true;
      initializeBinarySearchState(state);
      // printf("initialize bs state\n");
    }

    double currentTime = GetTime();
    if (shouldBinarySearch)
    {
      if (*state)
      {
        if (currentTime - *bsTime >= 2)
        {
          binarySearch(arr, atoi(inputBuffer), *state);
          *bsTime = currentTime;
        }
        if ((*state)->isDone)
        {
          // printf("binary search done\n");
          // printf("index is: %d\n", (*state)->answerIdx);
          char msg[20] = "";
          snprintf(msg, sizeof(msg), "index is: %d\n", (*state)->answerIdx);
          ShowToast((char *)msg);
          shouldBinarySearch = false;
        }
        else
        {
          draw_state(arr, (*state)->lo, (*state)->hi, (*state)->mid);
        }
      }
    }

    // persist with showing the user where the found value is
    if (*state)
    {
      draw_state(arr, (*state)->lo, (*state)->hi, (*state)->mid);
    }
  }
}

int visualizationTest(int *data)
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sort Visualizer");
  SetTargetFPS(FPS);
  // Set raygui font size
  GuiSetStyle(DEFAULT, TEXT_SIZE, 50);
  int currentSortChoice = 0;
  int sortDropdownActive = 0;
  float speed = 1.0;
  bool started = false;
  SortType sortChosen = NONE;

  initializeArray(data);
  BinarySearchState *bsState = NULL;
  void *sortState = NULL;
  double lastStepTime = GetTime();
  double lastBsTime = GetTime();

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
      Rectangle dropdownBounds = {startX + WIDGET_WIDTH + SPACING, y,
                                  WIDGET_WIDTH, WIDGET_HEIGHT};
      if (GuiDropdownBox(dropdownBounds, options, &currentSortChoice,
                         sortDropdownActive))
      {
        sortDropdownActive = !sortDropdownActive;
      }

      Rectangle buttonBounds = {startX + 2 * (WIDGET_WIDTH + SPACING), y,
                                WIDGET_WIDTH, WIDGET_HEIGHT};
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
      // printArray(data);

      cleanUpState(sortChosen, &sortState);

      // Visualization bars
      draw_state_with_color(data, -1, -1, -1, started ? YELLOW : BLUE);

      binarySearchUI(data, &lastBsTime, &bsState);
    }
    else if (started && !isSorted(data))
    {
      // printf("drawing it\n");
      DrawText(getSortTypeString(sortChosen), SCREEN_WIDTH / 2 - 100, 50, 40,
               YELLOW);
      // Visualization bars
      // draw_state_with_color(data, -1, -1, -1, started ? YELLOW : BLUE);
    }

    // Reset button
    Rectangle resetBounds = {startX + 3 * (WIDGET_WIDTH + SPACING), y,
                             WIDGET_WIDTH, WIDGET_HEIGHT};
    if (GuiButton(resetBounds, started ? "Reset" : "Randomize"))
    {
      started = false;
      // Reset your data array here, for example:
      initializeArray(data);
      resetSortState(sortChosen, data, &sortState);
      cleanUpBinarySearchState(&bsState);
      // clear binary search input string
      inputBuffer[0] = '\0';
    }

    DrawToast();
    EndDrawing();
  }

  // clean up states here
  if (bsState)
  {
    printf("cleaning up bs state.\n");
    cleanUpBinarySearchState(&bsState);
  }

  if (sortState)
  {
    printf("cleaning up sort state.\n");
    cleanUpState(sortChosen, &sortState);
  }

  CloseWindow();
  return EXIT_SUCCESS;
}

int main(void)
{
  // int *dummy = (int *)malloc(sizeof(int));
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
