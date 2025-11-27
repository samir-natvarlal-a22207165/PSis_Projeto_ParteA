#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include "config.h"

// Display context structure
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
} display_context;

// Initialize SDL and create window
// Returns pointer to display_context on success, NULL on error
display_context* display_init(const char *title, int width, int height);

// Clean up and destroy display
void display_destroy(display_context *ctx);

// Clear screen with background color (black)
void display_clear(display_context *ctx);

// Present/update the screen
void display_present(display_context *ctx);

#endif // DISPLAY_H