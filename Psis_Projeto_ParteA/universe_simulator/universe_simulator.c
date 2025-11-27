#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "display.h"

int main(int argc, char *argv[]) {
    universe_config config;
    display_context *display = NULL;
    const char *config_file = "universe.conf";

    // Allow custom config file via command line
    if (argc > 1) {
        config_file = argv[1];
    }

    printf("=== Space Trash - Universe Simulator ===\n");
    printf("Loading configuration from: %s\n", config_file);

    // Load configuration
    if (load_config(config_file, &config) != 0) {
        fprintf(stderr, "Failed to load configuration. Exiting.\n");
        return 1;
    }

    print_config(&config);

    // Initialize display
    display = display_init("Space Trash - Universe Simulator", 
                          config.universe_width, 
                          config.universe_height);
    
    if (!display) {
        fprintf(stderr, "Failed to initialize display. Exiting.\n");
        return 1;
    }

    printf("\nUniverse simulator started!\n");
    printf("Close the window to exit.\n");

    // Main loop (will be implemented in step 1c)
    int running = 1;
    SDL_Event event;

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            // ESC key to quit
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            }
        }

        // Clear screen
        display_clear(display);

        // TODO: Draw universe (planets, trash, etc.)

        // Present frame
        display_present(display);

        // Small delay to not consume 100% CPU
        SDL_Delay(10); // 10ms = 100 FPS (we'll use this for physics too)
    }

    // Cleanup
    display_destroy(display);
    printf("Universe simulator terminated.\n");

    return 0;
}