#include <SDL2/SDL.h>

#define SIZE 40 // grid size
#define CELL 20 // cell size
#define PADDED_SIZE (SIZE + 2) // Add padding of 1 on each side

int running = 1; 

// SDL / Graphics
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

// Game
int grid[PADDED_SIZE][PADDED_SIZE] = {0};
int timeflow = 0;  // Start paused

/* Graphics and Window */
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
        grid[y+1][x+1] = !grid[y+1][x+1];  // Offset by 1 for padding
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
    
    // Draw cells (skip padding)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (grid[y+1][x+1]) {  // Offset by 1 for padding
                SDL_Rect cell = {x*CELL+1, y*CELL+1, CELL-1, CELL-1};
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }

    // Draw pause/play indicator
    SDL_SetRenderDrawColor(renderer, !timeflow ? 255 : 0, !timeflow ? 0 : 255, 0, 255);
    SDL_Rect indicator = {SIZE*CELL - 20, 10, 10, 10};
    SDL_RenderFillRect(renderer, &indicator);

    SDL_RenderPresent(renderer);
}

/* Game Logic */

void update_padding() {
    // Copy edges to padding (wrap around)
    for (int i = 1; i <= SIZE; i++) {
        // Wrap horizontal edges
        grid[0][i] = grid[SIZE][i];         // Top padding
        grid[PADDED_SIZE-1][i] = grid[1][i];  // Bottom padding
        grid[i][0] = grid[i][SIZE];         // Left padding
        grid[i][PADDED_SIZE-1] = grid[i][1];  // Right padding
    }
    
    // Handle corners
    grid[0][0] = grid[SIZE][SIZE];                // Top-left
    grid[0][PADDED_SIZE-1] = grid[SIZE][1];         // Top-right
    grid[PADDED_SIZE-1][0] = grid[1][SIZE];         // Bottom-left
    grid[PADDED_SIZE-1][PADDED_SIZE-1] = grid[1][1];  // Bottom-right
}

int get_neighbors(int x, int y) {
    int neighbors = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            neighbors += grid[y+dy][x+dx];
        }
    }
    return neighbors;
}

void tick() {
    int newgrid[PADDED_SIZE][PADDED_SIZE] = {0};

    // Update only non-padding cells
    for (int y = 1; y <= SIZE; y++) {
        for (int x = 1; x <= SIZE; x++) {
            int neighbors = get_neighbors(x, y);
            if (neighbors == 3 || (grid[y][x] == 1 && neighbors == 2))
                newgrid[y][x] = 1;
        }
    }
    
    memcpy(grid, newgrid, sizeof(grid));
    update_padding();  // Update padding after each tick
}

int main() {
    init();
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEBUTTONDOWN) handle_click(event);
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) timeflow = !timeflow;
        }

        if (timeflow) {
            tick();
            SDL_Delay(50);  // 50ms delay when running
        }
        draw();
    }

    cleanup();
    return 0;
}