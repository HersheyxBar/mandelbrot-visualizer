#include <SDL2/SDL.h>
#include <complex.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_ITER 100
#define ZOOM_FACTOR 0.5

typedef struct {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    int max_iter;
    uint32_t* frame_buffer;
    bool is_dragging;
    int drag_start_x;
    int drag_start_y;
    bool just_clicked;  // New flag to track initial click
} MandelbrotState;

void init_state(MandelbrotState* state) {
    state->x_min = -2.0;
    state->x_max = 1.0;
    state->y_min = -1.5;
    state->y_max = 1.5;
    state->max_iter = MAX_ITER;
    state->frame_buffer = (uint32_t*)malloc(WIDTH * HEIGHT * sizeof(uint32_t));
    state->is_dragging = false;
    state->just_clicked = false;
}

uint32_t color_function(double i) {
    uint8_t r = (uint8_t)((1.0 - cos(i * 0.02)) * 127.5);
    uint8_t g = (uint8_t)((1.0 - cos(i * 0.015)) * 127.5);
    uint8_t b = (uint8_t)((1.0 - cos(i * 0.025)) * 127.5);
    
    return (r << 16) | (g << 8) | b;
}

uint32_t compute_pixel(MandelbrotState* state, double complex c) {
    double complex z = 0;
    int i;
    
    for (i = 0; i < state->max_iter && cabs(z) <= 2.0; i++) {
        z = z * z + c;
    }
    
    if (i == state->max_iter) {
        return 0;
    } else {
        double smooth_i = i + 1.0 - log2(log2(cabs(z)));
        return color_function(smooth_i);
    }
}

void render_mandelbrot(MandelbrotState* state) {
    clock_t start = clock();
    
    for (int y = 0; y < HEIGHT; y++) {
        double y_coord = state->y_min + ((double)y / HEIGHT) * (state->y_max - state->y_min);
        
        for (int x = 0; x < WIDTH; x++) {
            double x_coord = state->x_min + ((double)x / WIDTH) * (state->x_max - state->x_min);
            double complex c = x_coord + y_coord * I;
            
            state->frame_buffer[y * WIDTH + x] = compute_pixel(state, c);
        }
    }
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Frame time: %f seconds\n", time_spent);
}

void zoom(MandelbrotState* state, int x, int y, double factor) {
    // Convert screen coordinates to complex plane coordinates
    double x_coord = state->x_min + ((double)x / WIDTH) * (state->x_max - state->x_min);
    double y_coord = state->y_min + ((double)y / HEIGHT) * (state->y_max - state->y_min);
    
    // Calculate new ranges
    double x_range = (state->x_max - state->x_min) * factor;
    double y_range = (state->y_max - state->y_min) * factor;
    
    // Center the view on the clicked point
    state->x_min = x_coord - (x_range / 2.0);
    state->x_max = x_coord + (x_range / 2.0);
    state->y_min = y_coord - (y_range / 2.0);
    state->y_max = y_coord + (y_range / 2.0);
    
    if (factor < 1.0) {
        state->max_iter = (int)(state->max_iter * 1.1);
    }
}

void pan(MandelbrotState* state, int dx, int dy) {
    // Convert screen movement to complex plane movement
    double x_scale = (state->x_max - state->x_min) / WIDTH;
    double y_scale = (state->y_max - state->y_min) / HEIGHT;
    
    double x_offset = dx * x_scale;
    double y_offset = dy * y_scale;
    
    // Move the view
    state->x_min += x_offset;
    state->x_max += x_offset;
    state->y_min += y_offset;
    state->y_max += y_offset;
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow(
        "Mandelbrot Explorer (C)",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );
    
    MandelbrotState state;
    init_state(&state);
    
    printf("Controls:\n");
    printf("  Left click: Zoom in\n");
    printf("  Right click: Zoom out\n");
    printf("  Click and drag: Pan view\n");
    printf("  R: Reset view\n");
    printf("  ESC: Quit\n");
    
    render_mandelbrot(&state);
    
    bool running = true;
    SDL_Event event;
    int current_x, current_y;
    int movement_threshold = 5;  // Pixels of movement before starting drag
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        case SDLK_r:
                            init_state(&state);
                            render_mandelbrot(&state);
                            break;
                    }
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        state.is_dragging = true;
                        state.just_clicked = true;
                        state.drag_start_x = event.button.x;
                        state.drag_start_y = event.button.y;
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        zoom(&state, event.button.x, event.button.y, 2.0);
                        render_mandelbrot(&state);
                    }
                    break;
                    
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (state.just_clicked) {
                            // If it was just a click (no significant drag), then zoom
                            zoom(&state, event.button.x, event.button.y, ZOOM_FACTOR);
                            render_mandelbrot(&state);
                        }
                        state.is_dragging = false;
                        state.just_clicked = false;
                    }
                    break;
                    
                case SDL_MOUSEMOTION:
                    if (state.is_dragging) {
                        SDL_GetMouseState(&current_x, &current_y);
                        int dx = current_x - state.drag_start_x;
                        int dy = current_y - state.drag_start_y;
                        
                        // Check if we've moved enough to start dragging
                        if (abs(dx) > movement_threshold || abs(dy) > movement_threshold) {
                            state.just_clicked = false;  // We're definitely dragging now
                            pan(&state, -dx, -dy);
                            render_mandelbrot(&state);
                            state.drag_start_x = current_x;
                            state.drag_start_y = current_y;
                        }
                    }
                    break;
            }
        }
        
        SDL_UpdateTexture(texture, NULL, state.frame_buffer, WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    
    free(state.frame_buffer);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}