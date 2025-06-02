#include <SDL2/SDL.h>
#include <string.h>

#define SIZE 40
#define CELL 20

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
int running = 1;
int grid[SIZE][SIZE] = {0};
int timeflow = 0;
int speed = 100;

// Shift+select variables
int shift_pressed = 0;
int selection_start_x = -1;
int selection_start_y = -1;
int selection_active = 0;

void draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Grid lines
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    for (int i = 0; i <= SIZE; i++) {
        SDL_RenderDrawLine(renderer, i*CELL, 0, i*CELL, SIZE*CELL);
        SDL_RenderDrawLine(renderer, 0, i*CELL, SIZE*CELL, i*CELL);
    }
    
    // Highlight 4 center squares with light red, low opacity
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 80);  // Light red with low opacity
    int center = SIZE / 2;
    for (int dy = -1; dy <= 0; dy++) {
        for (int dx = -1; dx <= 0; dx++) {
            int x = center + dx;
            int y = center + dy;
            SDL_Rect highlight = {x*CELL+1, y*CELL+1, CELL-1, CELL-1};
            SDL_RenderFillRect(renderer, &highlight);
        }
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    // Cells
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SIZE; y++)
        for (int x = 0; x < SIZE; x++)
            if (grid[y][x]) {
                SDL_Rect cell = {x*CELL+1, y*CELL+1, CELL-1, CELL-1};
                SDL_RenderFillRect(renderer, &cell);
            }

    // Selection start indicator (blue dot when shift is pressed and selection is active)
    if (shift_pressed && selection_active && selection_start_x != -1 && selection_start_y != -1) {
        SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
        SDL_Rect start_indicator = {selection_start_x*CELL+CELL/4, selection_start_y*CELL+CELL/4, CELL/2, CELL/2};
        SDL_RenderFillRect(renderer, &start_indicator);
    }

    // Timeflow indicator
    SDL_SetRenderDrawColor(renderer, !timeflow ? 255 : 0, !timeflow ? 0 : 255, 0, 255);
    SDL_RenderFillRect(renderer, &(SDL_Rect){SIZE*CELL - 20, 10, 10, 10});
    SDL_RenderPresent(renderer);
}

void toggle_cell(int x, int y) {
    if (x >= 0 && x < SIZE && y >= 0 && y < SIZE)
        grid[y][x] = !grid[y][x];
}

void toggle_rectangle(int x1, int y1, int x2, int y2) {
    int min_x = x1 < x2 ? x1 : x2;
    int max_x = x1 > x2 ? x1 : x2;
    int min_y = y1 < y2 ? y1 : y2;
    int max_y = y1 > y2 ? y1 : y2;
    
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (x >= 0 && x < SIZE && y >= 0 && y < SIZE) {
                grid[y][x] = !grid[y][x];
            }
        }
    }
}

void handle_cell_click(int x, int y) {
    if (shift_pressed && selection_start_x != -1 && selection_start_y != -1) {
        // Shift+click: toggle rectangle from start to current position
        toggle_rectangle(selection_start_x, selection_start_y, x, y);
        selection_start_x = -1;
        selection_start_y = -1;
        selection_active = 0;
    } else {
        // Regular click: toggle single cell and set as new selection start
        toggle_cell(x, y);
        selection_start_x = x;
        selection_start_y = y;
        selection_active = 1;
    }
}

int get_neighbors(int x, int y) {
    int n = 0;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++)
            if (dx || dy)  // Skip the center cell
                n+= grid[(y + dy + SIZE) % SIZE][(x + dx + SIZE) % SIZE];
    return n;
}

void tick() {
    int newgrid[SIZE][SIZE] = {0};
    for (int y = 0; y < SIZE; y++)
        for (int x = 0; x < SIZE; x++) {
            int n = get_neighbors(x, y);
            if (n == 3 || (grid[y][x] && n == 2))
                newgrid[y][x] = 1;
        }
    memcpy(grid, newgrid, sizeof(grid));
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SIZE*CELL, SIZE*CELL, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEBUTTONDOWN) handle_cell_click(event.button.x/CELL, event.button.y/CELL);
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        timeflow = !timeflow;
                        break;
                    case SDLK_RIGHT:
                        speed = speed <= 0 ? 0 : speed - 10;
                        break;
                    case SDLK_LEFT:
                        speed = speed >= 1000 ? 1000 : speed + 10;
                        break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        shift_pressed = 1;
                        break;
                }
            }
            if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        shift_pressed = 0;
                        // Reset selection when shift is released
                        selection_start_x = -1;
                        selection_start_y = -1;
                        selection_active = 0;
                        break;
                }
            }
        }
        if (timeflow) {
            tick();
            SDL_Delay(speed);
        }
        draw();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}