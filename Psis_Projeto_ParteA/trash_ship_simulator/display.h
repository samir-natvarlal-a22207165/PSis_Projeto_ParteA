#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "config.h"
#include <stdbool.h>

// Display context structure
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Font *font_large;  // Large font for game over screen
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

// Set draw color (for drawing shapes)
void display_set_color(display_context *ctx, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// Draw a filled circle
void display_draw_circle(display_context *ctx, int x, int y, int radius);

// Draw a point/pixel
void display_draw_point(display_context *ctx, int x, int y);

// Draw text at position
void display_draw_text(display_context *ctx, const char *text, int x, int y, 
                       Uint8 r, Uint8 g, Uint8 b);

// ===== Game Object Drawing Functions =====

// Draw a planet at position
void display_draw_planet(display_context *ctx, float x, float y, char name, 
                         bool is_recycling, int num_trash, float radius);

// Draw trash at position
void display_draw_trash(display_context *ctx, float x, float y, float radius);

// Draw ship at position
void display_draw_ship(display_context *ctx, float x, float y, char name, int num_trash, float radius);

// Draw game over screen (universe collapsed)
void display_draw_game_over(display_context *ctx);

#endif // DISPLAY_H