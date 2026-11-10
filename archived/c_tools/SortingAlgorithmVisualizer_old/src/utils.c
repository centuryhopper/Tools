#include "../include/configs.h"
#include <ctype.h>
#include "../raylib/src/raylib.h"
#include <stdio.h>
#include <stdlib.h>
int getMaxValue(const int *arr, int size);


/*

sample test for why l <= h instead of l < h
[-1,0,3,5,9,12]

target =
9
*/

void printArray(int *arr) {
  for (int i = 0; i < ELEMENT_COUNT; i++) {
    printf("%d%s", arr[i], (i == ELEMENT_COUNT - 1) ? "\n" : " ");
  }
}

// initializes array with randomized values between 1 and 100
void initializeArray(int *arr) {
  for (int i = 0; i < ELEMENT_COUNT; ++i) {
    // just to show that quick sort runs in quadratic run time with this kind of
    // input
    // arr[i] = 100 - i;
    arr[i] = 1 + rand() % 100;
    // printf("%d%s", arr[i], (i == ELEMENT_COUNT - 1) ? "\n" : " ");
  }
}

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void str_to_lower(char *str) {
  while (*str) {
    *str = tolower((unsigned char)*str);
    str++;
  }
}

bool isSorted(const int *arr) {
  for (int i = 1; i < ELEMENT_COUNT; ++i) {
    if (arr[i] < arr[i - 1]) {
      return false; // Found a pair out of order
    }
  }
  return true; // All pairs in order
}

void draw_state_with_color(const int *arr, int red, int blue, int green,
                           Color color) {
  // ClearBackground(BLACK);

  // BeginDrawing();

  int barWidth = SCREEN_WIDTH / ELEMENT_COUNT;
  // Find the maximum value in the array for dynamic scaling
  int maxVal = getMaxValue(arr, ELEMENT_COUNT);

  for (int i = 0; i < ELEMENT_COUNT; ++i) {
    if (i == red)
      color = RED;
    else if (i == blue)
      color = BLUE;
    else if (i == green)
      color = GREEN;

    // Scale height based on max value so tallest bar reaches the top
    float scale = (float)(SCREEN_HEIGHT * 0.8f) / maxVal;
    int posX = i * barWidth;
    int barHeight = arr[i] * scale;
    int posY = SCREEN_HEIGHT - barHeight;
    DrawRectangle(posX, posY, barWidth - 1, barHeight, color);
  }

  // EndDrawing();
  // WaitTime(0.01);
}

void draw_state(const int *arr, int red, int blue, int green) {
  // ClearBackground(BLACK);

  // BeginDrawing();

  int barWidth = SCREEN_WIDTH / ELEMENT_COUNT;
  // Find the maximum value in the array for dynamic scaling
  int maxVal = getMaxValue(arr, ELEMENT_COUNT);

  for (int i = 0; i < ELEMENT_COUNT; ++i) {
    Color color = YELLOW;
    if (i == red)
      color = RED;
    else if (i == blue)
      color = BLUE;
    else if (i == green)
      color = GREEN;

    // Scale height based on max value so tallest bar reaches the top
    float scale = (float)(SCREEN_HEIGHT * 0.8f) / maxVal;
    int posX = i * barWidth;
    int barHeight = arr[i] * scale;
    int posY = SCREEN_HEIGHT - barHeight;
    DrawRectangle(posX, posY, barWidth - 1, barHeight, color);
  }

  // EndDrawing();
  // WaitTime(0.01);
}

int getMaxValue(const int *arr, int size) {
  int max = arr[0];
  for (int i = 1; i < size; i++) {
    if (arr[i] > max)
      max = arr[i];
  }
  return max;
}
