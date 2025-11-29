#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"
#include "display.h"
#include "universe-data.h"
#include "physics-rules.h"

// Game state structure
typedef struct {
    bool running;
    bool paused;
    bool game_over;       // Universe has collapsed
    display_context *display;
    universe_config config;
    universe_data *universe;
    int collision_count;  // Track number of collisions
} game_state;

// Initialize game state
game_state* game_init(const char *config_file) {
    game_state *state = (game_state*)malloc(sizeof(game_state));
    if (!state) {
        fprintf(stderr, "Failed to allocate game state\n");
        return NULL;
    }

    state->running = true;
    state->paused = false;
    state->game_over = false;
    state->display = NULL;
    state->universe = NULL;
    state->collision_count = 0;

    // Load configuration
    if (load_config(config_file, &state->config) != 0) {
        fprintf(stderr, "Failed to load configuration\n");
        free(state);
        return NULL;
    }

    print_config(&state->config);

    // Create universe
    state->universe = universe_create(&state->config);
    if (!state->universe) {
        fprintf(stderr, "Failed to create universe\n");
        free(state);
        return NULL;
    }

    // Initialize planets in the universe
    universe_initialize_planets(state->universe);

    // Initialize trash in the universe
    universe_initialize_trash(state->universe, state->config.initial_trash);

    // Print universe info
    universe_print_info(state->universe);

    // Initialize display
    state->display = display_init("Space Trash - Universe Simulator", 
                                  state->config.universe_width, 
                                  state->config.universe_height);
    
    if (!state->display) {
        fprintf(stderr, "Failed to initialize display\n");
        free(state);
        return NULL;
    }

    return state;
}

// Clean up game state
void game_destroy(game_state *state) {
    if (!state) return;

    if (state->display) {
        display_destroy(state->display);
    }

    if (state->universe) {
        universe_destroy(state->universe);
    }

    free(state);
}

// Handle SDL events
void handle_events(game_state *state) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                state->running = false;
                printf("Quit event received\n");
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        state->running = false;
                        printf("ESC pressed - exiting\n");
                        break;

                    case SDLK_SPACE:
                        if (!state->game_over) {
                            state->paused = !state->paused;
                            printf("Simulation %s\n", state->paused ? "PAUSED" : "RESUMED");
                        }
                        break;

                    case SDLK_q:
                        state->running = false;
                        printf("Q pressed - exiting\n");
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}

// Update game logic (physics, collisions, etc.)
void update_game(game_state *state) {
    if (state->paused || state->game_over) {
        return; // Skip updates when paused or game over
    }

    // Update physics (gravitational forces, velocity, position)
    update_physics(state->universe);

    // Check collisions between trash and planets
    // When trash hits planet center, NEW trash is spawned (original continues)
    check_trash_planet_collisions(state->universe);

    // Check if universe has collapsed
    if (universe_has_collapsed(state->universe)) {
        state->game_over = true;
        printf("\n");
        printf("═══════════════════════════════════════════\n");
        printf("  THE UNIVERSE HAS COLLAPSED!\n");
        printf("  Maximum trash reached: %d/%d\n", 
               universe_count_active_trash(state->universe),
               state->universe->max_trash);
        printf("  Humanity is doomed!\n");
        printf("═══════════════════════════════════════════\n");
        printf("\nPress ESC or Q to exit.\n");
    }
}

// Render the universe
void render_game(game_state *state) {
    // If game over, show red doom screen
    if (state->game_over) {
        display_draw_game_over(state->display);
        return;
    }

    // Normal game rendering
    // Clear screen (white background)
    display_clear(state->display);

    // Draw planets
    for (int i = 0; i < state->universe->num_planets; i++) {
        planet_structure *planet = universe_get_planet(state->universe, i);
        if (planet) {
            display_draw_planet(state->display, 
                              planet->x, 
                              planet->y, 
                              planet->name, 
                              i,  // index for label (A0-Z0, A1-Z1, etc.)
                              planet->is_recycling);
        }
    }

    // Draw trash
    for (int i = 0; i < state->universe->max_trash; i++) {
        trash_structure *trash = universe_get_trash(state->universe, i);
        if (trash) {
            display_draw_trash(state->display, trash->x, trash->y);
        }
    }

    // Draw info overlay (top-left corner)
    char info_text[64];
    snprintf(info_text, sizeof(info_text), "Trash: %d/%d", 
             universe_count_active_trash(state->universe),
             state->universe->max_trash);
    display_draw_text(state->display, info_text, 10, 10, 0, 0, 0);

    if (state->paused) {
        display_draw_text(state->display, "PAUSED", 10, 30, 255, 0, 0);
    }

    // Present the frame
    display_present(state->display);
}

// Main game loop
void game_loop(game_state *state) {
    Uint32 last_time = SDL_GetTicks();
    Uint32 current_time;
    Uint32 delta_time;
    const Uint32 TARGET_DELTA = 10; // 10ms per frame (100 FPS)

    printf("\n=== Universe Simulator Running ===\n");
    printf("Controls:\n");
    printf("  ESC or Q     - Quit\n");
    printf("  SPACE        - Pause/Resume\n");
    printf("  Close Window - Quit\n");
    printf("==================================\n\n");

    while (state->running) {
        current_time = SDL_GetTicks();
        delta_time = current_time - last_time;

        // Handle input events
        handle_events(state);

        // Update game logic (only if enough time has passed)
        if (delta_time >= TARGET_DELTA) {
            update_game(state);
            last_time = current_time;
        }

        // Render
        render_game(state);

        // Sleep to maintain frame rate and not consume 100% CPU
        // If we processed frame too fast, sleep for remaining time
        Uint32 frame_time = SDL_GetTicks() - current_time;
        if (frame_time < TARGET_DELTA) {
            SDL_Delay(TARGET_DELTA - frame_time);
        }
    }

    printf("\n=== Universe Simulator Stopped ===\n");
}

int main(int argc, char *argv[]) {
    const char *config_file = "universe.conf";

    // Allow custom config file via command line
    if (argc > 1) {
        config_file = argv[1];
    }

    printf("=== Space Trash - Universe Simulator ===\n");
    printf("Loading configuration from: %s\n\n", config_file);

    // Initialize game
    game_state *state = game_init(config_file);
    if (!state) {
        fprintf(stderr, "Failed to initialize game. Exiting.\n");
        return 1;
    }

    printf("\nInitialization successful!\n");

    // Run main game loop
    game_loop(state);

    // Cleanup
    game_destroy(state);
    printf("Universe simulator terminated cleanly.\n");

    return 0;
}