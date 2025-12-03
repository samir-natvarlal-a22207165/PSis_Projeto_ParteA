#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

display_context* display_init(const char *title, int width, int height) {
    // Allocate display context
    display_context *ctx = (display_context*)malloc(sizeof(display_context));
    if (!ctx) {
        fprintf(stderr, "Failed to allocate display context\n");
        return NULL;
    }

    ctx->width = width;
    ctx->height = height;
    ctx->window = NULL;
    ctx->renderer = NULL;
    ctx->font = NULL;
    ctx->font_large = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        free(ctx);
        return NULL;
    }

    // Initialize SDL_ttf for text rendering
    if (TTF_Init() < 0) {
        fprintf(stderr, "SDL_ttf initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        free(ctx);
        return NULL;
    }

    // Create window with title "GAME"
    ctx->window = SDL_CreateWindow(
        "GAME",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!ctx->window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        free(ctx);
        return NULL;
    }

    // Create renderer
    ctx->renderer = SDL_CreateRenderer(
        ctx->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!ctx->renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(ctx->window);
        TTF_Quit();
        SDL_Quit();
        free(ctx);
        return NULL;
    }

    // Try to load a default font (try multiple common paths)
    const char *font_paths[] = {
        "/System/Library/Fonts/Helvetica.ttc",           // macOS
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", // Linux/WSL
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", // Linux alternative
        "/Windows/Fonts/arial.ttf",                      // Windows (if mounted)
        NULL
    };

    for (int i = 0; font_paths[i] != NULL; i++) {
        ctx->font = TTF_OpenFont(font_paths[i], 14);
        if (ctx->font) {
            printf("Loaded font: %s\n", font_paths[i]);
            break;
        }
    }

    if (!ctx->font) {
        fprintf(stderr, "Warning: Could not load any font: %s\n", TTF_GetError());
        fprintf(stderr, "Text rendering will be disabled\n");
    }

    // Try to load a large font for game over screen (size 36)
    for (int i = 0; font_paths[i] != NULL; i++) {
        ctx->font_large = TTF_OpenFont(font_paths[i], 36);
        if (ctx->font_large) {
            printf("Loaded large font: %s (size 36)\n", font_paths[i]);
            break;
        }
    }

    // Set renderer draw color to white (for background)
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);

    printf("Display initialized: %dx%d\n", width, height);
    return ctx;
}

void display_destroy(display_context *ctx) {
    if (!ctx) return;

    if (ctx->font_large) {
        TTF_CloseFont(ctx->font_large);
    }

    if (ctx->font) {
        TTF_CloseFont(ctx->font);
    }

    if (ctx->renderer) {
        SDL_DestroyRenderer(ctx->renderer);
    }

    if (ctx->window) {
        SDL_DestroyWindow(ctx->window);
    }

    TTF_Quit();
    SDL_Quit();
    free(ctx);
    printf("Display destroyed\n");
}

void display_clear(display_context *ctx) {
    if (!ctx || !ctx->renderer) return;

    // Set color to white and clear (white background)
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
    SDL_RenderClear(ctx->renderer);
}

void display_present(display_context *ctx) {
    if (!ctx || !ctx->renderer) return;
    SDL_RenderPresent(ctx->renderer);
}

void display_set_color(display_context *ctx, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!ctx || !ctx->renderer) return;
    SDL_SetRenderDrawColor(ctx->renderer, r, g, b, a);
}

void display_draw_circle(display_context *ctx, int cx, int cy, int radius) {
    if (!ctx || !ctx->renderer) return;

    // Midpoint circle algorithm for filled circle
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderDrawPoint(ctx->renderer, cx + x, cy + y);
            }
        }
    }
}

void display_draw_point(display_context *ctx, int x, int y) {
    if (!ctx || !ctx->renderer) return;
    SDL_RenderDrawPoint(ctx->renderer, x, y);
}

void display_draw_text(display_context *ctx, const char *text, int x, int y,
                       Uint8 r, Uint8 g, Uint8 b) {
    if (!ctx || !ctx->renderer || !ctx->font || !text) return;

    SDL_Color color = {r, g, b, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(ctx->font, text, color);
    if (!surface) {
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(ctx->renderer, texture, NULL, &dest);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// ===== Game Object Drawing Functions =====

void display_draw_planet(display_context *ctx, float x, float y, char name, 
                        int index, bool is_recycling, int num_trash) {
    if (!ctx || !ctx->renderer) return;

    int radius = 20; // PLANET_RADIUS from universe-data.h
    int cx = (int)x;
    int cy = (int)y;

    // Set color based on recycling status
    if (is_recycling) {
        // Green for recycling planet
        display_set_color(ctx, 0, 200, 0, 255);
    } else {
        // Blue-purple for normal planets (RGB: 100, 100, 200)
        display_set_color(ctx, 100, 100, 200, 255);
    }

    // Draw filled circle
    display_draw_circle(ctx, cx, cy, radius);

    // number increments for trash
    // Letter cycles through A-Z(and other caracters)
    char letter = 'A' + (index % 26);
    int number = num_trash;
    
    char label[4];
    snprintf(label, sizeof(label), "%c%d", letter, number);
    
    // Position text at lower right of planet
    int text_x = cx + radius - 10;
    int text_y = cy + radius - 10;
    
    // Draw text in black
    display_draw_text(ctx, label, text_x, text_y, 0, 0, 0);
}


void display_draw_trash(display_context *ctx, float x, float y) {
    if (!ctx || !ctx->renderer) return;

    int trash_size = 3; // Small trash pieces
    int cx = (int)x;
    int cy = (int)y;

    // Red color for trash
    display_set_color(ctx, 255, 0, 0, 255);

    // Draw small filled circle
    display_draw_circle(ctx, cx, cy, trash_size);
}


void display_draw_ship(display_context *ctx, float x, float y, char name, 
                         int num_trash) {
    if (!ctx || !ctx->renderer) return;

    int radius = 20; // PLANET_RADIUS from universe-data.h
    int cx = (int)x;
    int cy = (int)y;

    // Set light red color for ship

    display_set_color(ctx, 100, 0, 0, 255);

    // Draw filled circle
    display_draw_circle(ctx, cx, cy, radius);

    // number increments for trash
    // Letter cycles through A-Z(and other caracters)
    char letter = name;
    int number = num_trash;
    
    char label[4];
    snprintf(label, sizeof(label), "%c%d", letter, number);
    
    // Position text at lower right of planet
    int text_x = cx + radius - 10;
    int text_y = cy + radius - 10;
    
    // Draw text in black
    display_draw_text(ctx, label, text_x, text_y, 0, 0, 0);
}


void display_draw_game_over(display_context *ctx) {
    if (!ctx || !ctx->renderer) return;

    // Fill entire screen with RED
    SDL_SetRenderDrawColor(ctx->renderer, 255, 0, 0, 255);
    SDL_RenderClear(ctx->renderer);

    // Draw the doom message in the center
    const char *message1 = "The Universe is full of trash!";
    const char *message2 = "Humanity is doomed!";

    // Use large font if available, otherwise use regular font
    TTF_Font *font_to_use = ctx->font_large ? ctx->font_large : ctx->font;

    if (font_to_use) {
        SDL_Color black = {0, 0, 0, 255};
        
        // Render first line
        SDL_Surface *surface1 = TTF_RenderText_Solid(font_to_use, message1, black);
        if (surface1) {
            SDL_Texture *texture1 = SDL_CreateTextureFromSurface(ctx->renderer, surface1);
            if (texture1) {
                // Center the text
                SDL_Rect dest1 = {
                    (ctx->width - surface1->w) / 2,
                    (ctx->height / 2) - 40,
                    surface1->w,
                    surface1->h
                };
                SDL_RenderCopy(ctx->renderer, texture1, NULL, &dest1);
                SDL_DestroyTexture(texture1);
            }
            SDL_FreeSurface(surface1);
        }

        // Render second line
        SDL_Surface *surface2 = TTF_RenderText_Solid(font_to_use, message2, black);
        if (surface2) {
            SDL_Texture *texture2 = SDL_CreateTextureFromSurface(ctx->renderer, surface2);
            if (texture2) {
                // Center the text
                SDL_Rect dest2 = {
                    (ctx->width - surface2->w) / 2,
                    (ctx->height / 2) + 10,
                    surface2->w,
                    surface2->h
                };
                SDL_RenderCopy(ctx->renderer, texture2, NULL, &dest2);
                SDL_DestroyTexture(texture2);
            }
            SDL_FreeSurface(surface2);
        }
    }

    SDL_RenderPresent(ctx->renderer);
}