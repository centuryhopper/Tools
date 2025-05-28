#ifndef UTILS_H
#define UTILS_H
#include "raylib.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int getMaxValue(const int *arr, int size);
void swap(int *a, int *b);
void draw_state_with_color(const int *arr, int red, int blue, int green, Color color);
void draw_state(const int* arr, int red, int blue, int green);
void str_to_lower(char *str);
bool isSorted(const int *arr);
void printArray(const int* arr);
void initializeArray(int* arr);

#endif