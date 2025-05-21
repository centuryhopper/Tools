#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <random>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/*
Compile C++ to webassembly:

https://emscripten.org/docs/getting_started/downloads.html
*/

// emcc test.cpp -std=c++20 -s USE_SDL=2 -s WASM=1 -O2 -o index.html && emrun --no_browser --port 8080 .

// --- App State ---
struct SortVisualizer {
    SDL_Renderer* renderer = nullptr;
    std::vector<int> data;
    int outer = 99;
    int inner = 1;
    bool sorted = false;
};

SortVisualizer app;

// --- Utility Functions ---
void draw_state(const std::vector<int>& v, SDL_Renderer* renderer, int red, int blue) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear
    SDL_RenderClear(renderer);

    for (int i = 0; i < v.size(); ++i) {
        if (i == red)
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red marker
        else if (i == blue)
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue marker
        else
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Default

        SDL_RenderDrawLine(renderer, i, 99, i, 99 - v[i]);
    }

    SDL_RenderPresent(renderer);
}

void bubble_sort_step() {
    if (app.sorted) return;

    // Handle quit event
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
#endif
            SDL_Quit();
            exit(0);
        }
    }

    if (app.inner <= app.outer) {
        if (app.data[app.inner] < app.data[app.inner - 1]) {
            std::swap(app.data[app.inner], app.data[app.inner - 1]);
        }
        draw_state(app.data, app.renderer, app.outer, app.inner);
        app.inner++;
    } else {
        app.outer--;
        app.inner = 1;

        if (app.outer <= 0) {
            app.sorted = true;
            draw_state(app.data, app.renderer, -1, -1); // Final clean draw
        }
    }
}

// --- Entry Point ---
int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_CreateWindowAndRenderer(1000, 1000, 0, &window, &app.renderer);
    SDL_RenderSetScale(app.renderer, 10, 10);

    // Generate random data
    std::random_device rd;
    std::uniform_int_distribution dist(1, 99);
    app.data.reserve(100);
    for (int i = 0; i < 100; ++i)
        app.data.push_back(dist(rd));

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(bubble_sort_step, 0, 1);
#else
    while (!app.sorted) {
        bubble_sort_step();
        SDL_Delay(10);
    }

    // Keep window open after sort
    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
                quit = true;
        }
    }
#endif

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
