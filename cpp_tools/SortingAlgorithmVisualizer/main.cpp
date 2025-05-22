// main.cpp
#include <SDL2/SDL.h>
#include <iostream>
#include <random>
#include <ranges>
#include <algorithm>
#include <iterator>

void draw_state(std::vector<int> &v, SDL_Renderer *renderer, unsigned int red, unsigned int blue);
void ResetColors(SDL_Renderer *renderer, std::vector<int> &v);

/*
    sudo apt install libc++-dev libc++abi-dev
    g++ main.cpp -std=c++20 -o main $(sdl2-config --cflags --libs) && ./main

    check for memory leaks:
        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main


    All sorting algorithms in this file will sort the vector in place as this shows the best consistency in the animations

    Run this command before using emcc:
        source ~/emsdk/emsdk_env.fish

    emcc test.cpp -std=c++20 -s USE_SDL=2 -s WASM=1 -O2 -o index.html && emrun --no_browser --port 8080 .
*/

class SortingAlorithms
{
private:
    static void mergeyMerge(std::vector<int> &v, int left, int mid, int right, SDL_Renderer *renderer)
    {
        std::vector<int> tmp(right - left + 1);
        int i = left, j = mid + 1, k = 0;

        while (i <= mid && j <= right)
        {
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    SDL_Quit();
                    exit(0);
                }
            }

            if (v[i] <= v[j])
                tmp[k++] = v[i++];
            else
                tmp[k++] = v[j++];

            if (renderer)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                draw_state(v, renderer, i, j);
                SDL_RenderPresent(renderer);
                SDL_Delay(1);
            }
        }

        while (i <= mid)
            tmp[k++] = v[i++];
        while (j <= right)
            tmp[k++] = v[j++];

        // copy tmp array contents over to original v
        for (int p = 0; p < tmp.size(); ++p)
        {
            v[left + p] = tmp[p];

            if (renderer)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                draw_state(v, renderer, left + p, -1);
                SDL_RenderPresent(renderer);
                SDL_Delay(1);
            }
        }
    }

    static int quickSortPartition(int* arr, int size, int lo, int hi)
    {
        if (hi >= size)
        {
            printf("hi exceeded bounds\n");
        }
        if (lo < 0)
        {
            printf("lo is negative\n");
        }
        int pivot = lo;
        int i = lo;
        int j = hi;
        int left, right;
        while (i < j)
        {
            do {
                i+=1;
                left = arr[i];
            } while (left <= arr[pivot]);
            do {
                j-=1;
                right = (j < size) ? arr[j] : INT32_MAX;
            } while (right > arr[pivot]);

            if (i < j)
            {
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[j], arr[pivot]);
        return j;
    }

public:
    /*
    Type: Bubble Sort

    Best Case: O(n) (if optimized with a swapped flag — not done here)

    Average/Worst Case: O(n²)

    Stable: Yes

    In-place: Yes
    */
    static void bubbleSort(std::vector<int> &v, SDL_Renderer *renderer)
    {
        if (v.size() < 2)
            return;
        for (int i = v.size() - 1; i >= 0; i--)
        {
            bool swapped = false;
            for (int j = 1; j <= i; j++)
            {
                // ✅ Handle events
                SDL_Event e;
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_QUIT)
                    {
                        SDL_Quit();
                        exit(0);
                    }
                }

                if (v[j] < v[j - 1])
                {
                    std::swap(v[j], v[j - 1]);
                    swapped = true;
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                draw_state(v, renderer, i, j);
                SDL_RenderPresent(renderer);
                SDL_Delay(1);
            }
            if (!swapped)
                break; // list is sorted
        }
    }

    static void mergeSort(std::vector<int> &v, int left, int right, SDL_Renderer *renderer)
    {
        if (left >= right)
            return;

        // postorder style traversal
        auto mid = left + (right - left) / 2;
        mergeSort(v, left, mid, renderer);
        mergeSort(v, mid + 1, right, renderer);
        mergeyMerge(v, left, mid, right, renderer);
    }

    static void selectionSort(int *arr, int size)
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
            std::swap(arr[i], arr[minIdx]);
        }
    }

    static void insertionSort(int* arr, int size)
    {
        for (int i = 0; i < size; i++)
        {
            for (int j = i + 1; j > 0; j--)
            {
                if (arr[j] < arr[j-1])
                {
                    std::swap(arr[j-1], arr[j]);
                }
            }
        }
    }

    static void quickSort(int* arr, int size, int lo, int hi)
    {
        if (lo >= hi)
        {
            return;
        }
        int partitionIdx = quickSortPartition(arr, size, lo, hi);
        quickSort(arr,size,lo,partitionIdx);
        quickSort(arr,size,partitionIdx+1,hi);
    }
};

void ResetColors(SDL_Renderer *renderer, std::vector<int> &v)
{
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Re-draw everything in white (no highlights)
    draw_state(v, renderer, -1, -1); // -1 means no bar gets red/blue

    SDL_RenderPresent(renderer);
}

void draw_state(std::vector<int> &v, SDL_Renderer *renderer, unsigned int red, unsigned int blue)
{
    auto idx = 0;
    for (int value : v)
    {
        if (idx == red)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        else if (idx == blue)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }

        // lines to represent the bars
        /*
        (0,0) ------------------> x
        |
        |
        |
        v
        y
        */
        // Smaller y2 ⇒ line ends higher up ⇒ taller bar.
        SDL_RenderDrawLine(renderer, idx, 99, idx, 99 - value);
        idx += 1;
    }
}

void cleanUp(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                }
            }
        }
    }

    // 🧼 Clean up SDL resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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

int main()
{
    // if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    //     std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    //     return 1;
    // }

    // SDL_Window* window = nullptr;
    // SDL_Renderer* renderer = nullptr;

    // SDL_CreateWindowAndRenderer(100*10, 100*10, 0, &window, &renderer);
    // SDL_RenderSetScale(renderer, 10, 10);

    // std::vector<int> v = {2,8,5,3,9,4,1};
    std::random_device rd;
    std::uniform_int_distribution d(1, 99);
    // std::vector<int> v;
    // int *arr = (int *)calloc(sizeof(int), 100);
    // if (!arr)
    // {
    //     exit(1);
    // }
    int arr[100];
    for (int i = 0; i < 100; i++)
    {
        // v.push_back(d(rd));
        arr[i] = d(rd);
        // printf("%d%s", arr[i], (i==99) ? "\n": " ");
    }

    // SortingAlorithms::mergeSort(v,0,v.size(), renderer);
    // SortingAlorithms::bubbleSort(v,renderer);
    // SortingAlorithms::selectionSort(arr, 100);
    // SortingAlorithms::insertionSort(arr, 100);
    // printf("%d\n", sizeof(arr) / sizeof(arr[0]));
    SortingAlorithms::quickSort(arr, sizeof(arr) / sizeof(arr[0]), 0, 100);

    for (int i = 0; i < 100; i++)
    {
        printf("%d%s", arr[i], (i == 99) ? "\n" : " ");
    }

    int size = sizeof(arr) / sizeof(arr[0]);
    // printf("size: %d\n", size);

    if (isSorted(arr, size))
    {
        std::cout << "Sorted\n";
    }
    else
    {
        std::cout << "Not sorted\n";
    }

    // ResetColors(renderer, v);

    // std::copy(v.begin(), v.end(),
    //           std::ostream_iterator<int>(std::cout, " "));
    // if (std::ranges::is_sorted(v))
    // {
    //     std::cout << "sorted" << std::endl;
    // }

    // free(arr);
    // cleanUp(window, renderer);

    return 0;
}
