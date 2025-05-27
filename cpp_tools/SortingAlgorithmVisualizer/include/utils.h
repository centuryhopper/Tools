#ifndef UTILS_H
#define UTILS_H
#include "raylib.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int getMaxValue(const int *arr, int size);
void draw_state_with_color(const int *arr, int red, int blue, int green, Color color);
void draw_state(const int* arr, int red, int blue, int green);

#endif