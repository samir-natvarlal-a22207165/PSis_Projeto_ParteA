#include "display.h"
#include <stdio.h>
#include <stdlib.h>

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

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        free(ctx);
        return NULL;
    }

    // Create window
    ctx->window = SDL_CreateWindow(
        title,
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
        SDL_Quit();
        free(ctx);
        return NULL;
    }

    // Set renderer draw color to black (for clearing)
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);

    printf("Display initialized: %dx%d\n", width, height);
    return ctx;
}

void display_destroy(display_context *ctx) {
    if (!ctx) return;

    if (ctx->renderer) {
        SDL_DestroyRenderer(ctx->renderer);
    }

    if (ctx->window) {
        SDL_DestroyWindow(ctx->window);
    }

    SDL_Quit();
    free(ctx);
    printf("Display destroyed\n");
}

void display_clear(display_context *ctx) {
    if (!ctx || !ctx->renderer) return;

    // Set color to black and clear
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
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