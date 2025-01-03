#include <SDL2/SDL.h>
#include <complex.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 1920
#define HEIGHT 1080
#define MAX_ITER 100
#define PREVIEW_BLOCK_SIZE 8  // Size of preview blocks (smaller = better quality but slower)
#define ZOOM_FACTOR 0.5

typedef struct {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    double camera_x;
    double camera_y;
    double zoom_level;
    int max_iter;
    uint32_t* frame_buffer;
    bool is_dragging;
    int drag_start_x;
    int drag_start_y;
    bool just_clicked;
} MandelbrotState;

void render_mandelbrot_progressive(MandelbrotState* state, SDL_Renderer* renderer, SDL_Texture* texture);
void update_view_bounds(MandelbrotState* state);

void update_view_bounds(MandelbrotState* state) {
    double view_width = 3.0 / state->zoom_level;
    double view_height = view_width * ((double)HEIGHT / WIDTH);
    
    state->x_min = state->camera_x - view_width/2;
    state->x_max = state->camera_x + view_width/2;
    state->y_min = state->camera_y - view_height/2;
    state->y_max = state->camera_y + view_height/2;
}

void init_state(MandelbrotState* state) {
    state->camera_x = -0.5;
    state->camera_y = 0.0;
    state->zoom_level = 1.0;
    state->max_iter = MAX_ITER;
    state->frame_buffer = (uint32_t*)malloc(WIDTH * HEIGHT * sizeof(uint32_t));
    state->is_dragging = false;
    state->just_clicked = false;
    update_view_bounds(state);
}

uint32_t color_function(double i) {
    double r = sin(0.1 * i + 0.0) * 127.0 + 128.0;
    double g = sin(0.1 * i + 2.1) * 127.0 + 128.0;
    double b = sin(0.1 * i + 4.2) * 127.0 + 128.0;
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

uint32_t compute_pixel(MandelbrotState* state, double complex c) {
    long double complex z = c;
    int i;
    double epsilon = 1e-14 / state->zoom_level;
    
    for (i = 0; i < state->max_iter; i++) {
        z = z * z + c;
        if (cabsl(z) > 2.0) break;
        if (cabsl(z) < epsilon) return 0;
    }

    if (i == state->max_iter) {
        return 0;
    } else {
        double smooth_i = i + 1.0 - log2l(log2l(cabsl(z)));
        return color_function(smooth_i);
    }
}

void render_mandelbrot_progressive(MandelbrotState* state, SDL_Renderer* renderer, SDL_Texture* texture) {
    clock_t render_start = clock();
    
    // Fast preview pass
    #pragma omp parallel for collapse(2) schedule(dynamic, 32)
    for (int y = 0; y < HEIGHT; y += PREVIEW_BLOCK_SIZE) {
        for (int x = 0; x < WIDTH; x += PREVIEW_BLOCK_SIZE) {
            double x_coord = state->x_min + ((double)(x + PREVIEW_BLOCK_SIZE/2) / WIDTH) * (state->x_max - state->x_min);
            double y_coord = state->y_min + ((double)(y + PREVIEW_BLOCK_SIZE/2) / HEIGHT) * (state->y_max - state->y_min);
            double complex c = x_coord + y_coord * I;
            uint32_t color = compute_pixel(state, c);
            
            for (int by = 0; by < PREVIEW_BLOCK_SIZE && y + by < HEIGHT; by++) {
                for (int bx = 0; bx < PREVIEW_BLOCK_SIZE && x + bx < WIDTH; bx++) {
                    state->frame_buffer[(y + by) * WIDTH + (x + bx)] = color;
                }
            }
        }
    }
    
    // Update preview
    SDL_UpdateTexture(texture, NULL, state->frame_buffer, WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    // Full quality pass - process multiple rows at once for better cache utilization
    #pragma omp parallel for collapse(2) schedule(dynamic, 32)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double x_coord = state->x_min + ((double)x / WIDTH) * (state->x_max - state->x_min);
            double y_coord = state->y_min + ((double)y / HEIGHT) * (state->y_max - state->y_min);
            double complex c = x_coord + y_coord * I;
            state->frame_buffer[y * WIDTH + x] = compute_pixel(state, c);
        }
    }
    
    // Final update
    SDL_UpdateTexture(texture, NULL, state->frame_buffer, WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    clock_t render_end = clock();
    double render_time = (double)(render_end - render_start) / CLOCKS_PER_SEC;
    printf("\rFrame time: %.3f seconds | Zoom: %.2e | Center: %.10f + %.10fi", 
           render_time, state->zoom_level, state->camera_x, state->camera_y);
    fflush(stdout);
}

void zoom(MandelbrotState* state, int x, int y, double factor) {
    double x_coord = state->x_min + ((double)x / WIDTH) * (state->x_max - state->x_min);
    double y_coord = state->y_min + ((double)y / HEIGHT) * (state->y_max - state->y_min);
    
    state->camera_x = x_coord;
    state->camera_y = y_coord;
    
    state->zoom_level *= (factor < 1.0) ? 1/factor : factor;
    
    if (factor < 1.0) {
        state->max_iter = (int)(state->max_iter * 1.1);
        if (state->max_iter > 100000) state->max_iter = 100000;
    }
    
    update_view_bounds(state);
}

void pan(MandelbrotState* state, int dx, int dy) {
    // Convert screen movement to complex plane movement
    double x_scale = (state->x_max - state->x_min) / WIDTH;
    double y_scale = (state->y_max - state->y_min) / HEIGHT;

    // Scale the movement based on zoom level for consistent feel
    state->camera_x += dx * x_scale;  // Changed from -= to +=
    state->camera_y += dy * y_scale;  // Changed from -= to +=
    
    update_view_bounds(state);
}

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Infinite Zoom Mandelbrot Explorer",
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

    printf("\nInfinite Zoom Mandelbrot Explorer\n");
    printf("Controls:\n");
    printf("  Left click: Zoom in\n");
    printf("  Right click: Zoom out\n");
    printf("  Click and drag: Pan view\n");
    printf("  R: Reset view\n");
    printf("  ESC: Quit\n\n");

    render_mandelbrot_progressive(&state, renderer, texture);

    bool running = true;
    SDL_Event event;
    int current_x, current_y;
    int movement_threshold = 5;

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
                            render_mandelbrot_progressive(&state, renderer, texture);
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
                        render_mandelbrot_progressive(&state, renderer, texture);
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (state.just_clicked) {
                            zoom(&state, event.button.x, event.button.y, ZOOM_FACTOR);
                            render_mandelbrot_progressive(&state, renderer, texture);
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

                        if (abs(dx) > 0 || abs(dy) > 0) {
                            state.just_clicked = false;
                            pan(&state, -dx, -dy);
                            state.drag_start_x = current_x;
                            state.drag_start_y = current_y;
                            render_mandelbrot_progressive(&state, renderer, texture);
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