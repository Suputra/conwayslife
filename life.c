#include <SDL2/SDL.h>

#define SIZE 40 // grid size
#define CELL 20 // cell size

int grid[SIZE][SIZE] = {0};
SDL_Window* window;
SDL_Renderer* renderer;

void init() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZE*CELL, SIZE*CELL, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);
}

void cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void toggle_cell(int x, int y) {
    if (x >= 0 && x < SIZE && y >= 0 && y < SIZE) {
        grid[y][x] = !grid[y][x];
    }
}

void handle_click(SDL_Event event) {
    int x = event.button.x / CELL;
    int y = event.button.y / CELL;
    toggle_cell(x, y);
}

void draw() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Draw grid
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    for (int i = 0; i <= SIZE; i++) {
        SDL_RenderDrawLine(renderer, i*CELL, 0, i*CELL, SIZE*CELL);
        SDL_RenderDrawLine(renderer, 0, i*CELL, SIZE*CELL, i*CELL);
    }
    
    // Draw cells
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (grid[y][x]) {
                SDL_Rect cell = {x*CELL+1, y*CELL+1, CELL-1, CELL-1};
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

int main() {
    init();
    
    SDL_Event event;
    int running = 1;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEBUTTONDOWN) handle_click(event);
        }
        draw();
    }
    
    cleanup();
    return 0;
}