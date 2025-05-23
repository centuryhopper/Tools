#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "../include/configs.h"
#include "../include/bubble_sort.h"
#include "../include/insertion_sort.h"
#include "../include/selection_sort.h"
#include "../include/merge_sort.h"
#include "../include/quick_sort.h"
#include "../include/draw_state.h"


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

void str_to_lower(char *str)
{
    while (*str)
    {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

bool isSorted(const int *arr, int n)
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

static void insertionSortRaw(int *arr, int size)
{
    if (size < 2)
        return;
    // make sure j doesn't overshoot
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j > 0; j--)
        {
            if (arr[j] < arr[j - 1])
            {
                int tmp = arr[j - 1];
                arr[j - 1] = arr[j];
                arr[j] = tmp;
            }
        }
    }
}

static void bubbleSortRaw(int *arr, int size)
{
    if (size < 2)
        return;
    for (int i = size - 1; i >= 0; i--)
    {
        bool swapped = false;
        for (int j = 1; j <= i; j++)
        {
            if (arr[j] < arr[j - 1])
            {
                int tmp = arr[j];
                arr[j] = arr[j - 1];
                arr[j - 1] = tmp;
                swapped = true;
            }
        }
        if (!swapped)
            break; // list is sorted
    }
}

static void selectionSortRaw(int *arr, int size)
{
    if (size < 2)
        return;
    for (int i = 0; i < size; i++)
    {
        // assume ith index is the min so far
        int minIdx = i;
        for (int j = i + 1; j < size; j++)
        {
            if (arr[minIdx] > arr[j])
            {
                minIdx = j;
            }
        }
        int tmp = arr[i];
        arr[i] = arr[minIdx];
        arr[minIdx] = tmp;
    }
}

static int quickSortPartition(int *arr, int size, int lo, int hi)
{
    // if (hi >= size)
    // {
    //     printf("hi exceeded bounds\n");
    // }
    // if (lo < 0)
    // {
    //     printf("lo is negative\n");
    // }
    int pivot = lo;
    int i = lo;
    int j = hi;
    int left, right;
    while (i < j)
    {
        do
        {
            i += 1;
            left = arr[i];
        } while (left <= arr[pivot]);
        do
        {
            j -= 1;
            right = (j < size) ? arr[j] : INT_MAX;
        } while (right > arr[pivot]);

        if (i < j)
        {
            int tmp = arr[j];
            arr[j] = arr[i];
            arr[i] = tmp;
        }
    }
    int tmp = arr[j];
    arr[j] = arr[pivot];
    arr[pivot] = tmp;
    return j;
}

static void quickSortRaw(int *arr, int size, int lo, int hi)
{
    if (lo >= hi)
    {
        return;
    }
    int partitionIdx = quickSortPartition(arr, size, lo, hi);
    quickSortRaw(arr, size, lo, partitionIdx);
    quickSortRaw(arr, size, partitionIdx + 1, hi);
}

static void mergeyMerge(int *arr, int left, int mid, int right)
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

static void mergeSortRaw(int *arr, int left, int right)
{
    if (left >= right)
        return;

    // postorder style traversal
    int mid = left + (right - left) / 2;
    mergeSortRaw(arr, left, mid);
    mergeSortRaw(arr, mid + 1, right);
    mergeyMerge(arr, left, mid, right);
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
        fprintf(stderr, "Error: Unknown argument. Please choose one of the following: (s,i,b,q,m)\n");
        return EXIT_FAILURE; // Non-zero to indicate error
    }

    for (int i = 0; i < 100; i++)
    {
        printf("%d%s", data[i], (i == 99) ? "\n" : " ");
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
        fprintf(stderr, "Error: Unknown argument. Please choose one of the following: (s,i,b,q,m)\n");
        return EXIT_FAILURE; // Non-zero to indicate error
    }

    for (int i = 0; i < 100; i++)
    {
        printf("%d%s", data[i], (i == 99) ? "\n" : " ");
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

int main(int argc, char *argv[])
{
    // printf("Number of arguments: %d\n", argc);
    // for (int i = 0; i < argc; i++) {
    //     printf("Argument %d: %s\n", i, argv[i]);
    // }

    if (argc < 2)
    {
        fprintf(stderr, "Error: Missing argument. Usage: %s <type of sort>\n", argv[0]);
        return EXIT_FAILURE; // Non-zero to indicate error
    }

    printf("raw test? (y/n)\n");
    char input;
    scanf(" %c", &input); // ✅ Recommended for reading single characters
    input = tolower(input);

    int data[ELEMENT_COUNT];
    srand((unsigned int)time(NULL));
    for (int i = 0; i < ELEMENT_COUNT; ++i)
    {
        data[i] = 100 - i;
        // data[i] = 1 + rand() % 100;
    }

    switch (input)
    {
        case 'y':
            rawTest(argv[1], data);
            break;
        case 'n':
            InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sort Visualizer - raylib");
            SetTargetFPS(60);
            visualizationTest(argv[1], data);
            while (!WindowShouldClose())
            {
                BeginDrawing();
                draw_state(data, -1, -1, -1); // Final state
                EndDrawing();
            }
            CloseWindow();
            break;
        default:
            fprintf(stderr, "Error: Invalid input\n");
            break;
    }

    return 0;
}
