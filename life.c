#include <SDL2/SDL.h>

#define SIZE 40
#define CELL 20

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
int running = 1;
int grid[SIZE][SIZE] = {0};
int timeflow = 0;
int speed = 100;

void draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Grid lines
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    for (int i = 0; i <= SIZE; i++) {
        SDL_RenderDrawLine(renderer, i*CELL, 0, i*CELL, SIZE*CELL);
        SDL_RenderDrawLine(renderer, 0, i*CELL, SIZE*CELL, i*CELL);
    }
    
    // Cells
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SIZE; y++)
        for (int x = 0; x < SIZE; x++)
            if (grid[y][x]) {
                SDL_Rect cell = {x*CELL+1, y*CELL+1, CELL-1, CELL-1};
                SDL_RenderFillRect(renderer, &cell);
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
            if (event.type == SDL_MOUSEBUTTONDOWN) toggle_cell(event.button.x/CELL, event.button.y/CELL);
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) timeflow = !timeflow;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT) speed = speed <= 0 ? 0 : speed - 10;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT) speed = speed >= 1000 ? 1000 : speed + 10;
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