#include "config.h"
#include <libconfig.h>
#include <stdio.h>
#include <stdlib.h>

int load_config(const char *filename, universe_config *config) {
    config_t cfg;
    config_init(&cfg);

    // Try to read the configuration file
    if (!config_read_file(&cfg, filename)) {
        fprintf(stderr, "Error reading config file '%s': %s at line %d\n",
                filename,
                config_error_text(&cfg),
                config_error_line(&cfg));
        config_destroy(&cfg);
        return -1;
    }

    // Read universe_width
    if (config_lookup_int(&cfg, "universe_width", &config->universe_width) == CONFIG_FALSE) {
        fprintf(stderr, "Missing parameter: universe_width\n");
        config_destroy(&cfg);
        return -1;
    }

    // Read universe_height
    if (config_lookup_int(&cfg, "universe_height", &config->universe_height) == CONFIG_FALSE) {
        fprintf(stderr, "Missing parameter: universe_height\n");
        config_destroy(&cfg);
        return -1;
    }

    // Read num_planets
    if (config_lookup_int(&cfg, "num_planets", &config->num_planets) == CONFIG_FALSE) {
        fprintf(stderr, "Missing parameter: num_planets\n");
        config_destroy(&cfg);
        return -1;
    }

    // Read max_trash
    if (config_lookup_int(&cfg, "max_trash", &config->max_trash) == CONFIG_FALSE) {
        fprintf(stderr, "Missing parameter: max_trash\n");
        config_destroy(&cfg);
        return -1;
    }

    // Read initial_trash
    if (config_lookup_int(&cfg, "initial_trash", &config->initial_trash) == CONFIG_FALSE) {
        fprintf(stderr, "Missing parameter: initial_trash\n");
        config_destroy(&cfg);
        return -1;
    }

    // Read ship_capacity
    if (config_lookup_int(&cfg, "ship_capacity", &config->ship_capacity) == CONFIG_FALSE) {
        fprintf(stderr, "Missing parameter: ship_capacity\n");
        config_destroy(&cfg);
        return -1;
    }

        // Read ship_capacity
    if (config_lookup_int(&cfg, "max_ships", &config->max_ships) == CONFIG_FALSE) {
        fprintf(stderr, "Missing parameter: max_ships\n");
        config_destroy(&cfg);
        return -1;
    }

    // Validate values
    if (config->universe_width <= 0 || config->universe_height <= 0) {
        fprintf(stderr, "Error: Universe dimensions must be positive\n");
        config_destroy(&cfg);
        return -1;
    }

    if (config->num_planets <= 0 || config->num_planets > 26) {
        fprintf(stderr, "Error: Number of planets must be between 1 and 26\n");
        config_destroy(&cfg);
        return -1;
    }

    if (config->max_trash <= 0) {
        fprintf(stderr, "Error: Max trash must be positive\n");
        config_destroy(&cfg);
        return -1;
    }

    if (config->initial_trash < 0 || config->initial_trash > config->max_trash) {
        fprintf(stderr, "Error: Initial trash must be between 0 and max_trash\n");
        config_destroy(&cfg);
        return -1;
    }

    if (config->ship_capacity <= 0) {
        fprintf(stderr, "Error: Ship capacity must be positive\n");
        config_destroy(&cfg);
        return -1;
    }

    if (config->ship_capacity <= 0) {
        fprintf(stderr, "Error: Max ships must be positive\n");
        config_destroy(&cfg);
        return -1;
    }

    config_destroy(&cfg);
    return 0;
}

void print_config(universe_config *config) {
    printf("=== Universe Configuration ===\n");
    printf("Universe dimensions: %dx%d\n", config->universe_width, config->universe_height);
    printf("Number of planets: %d\n", config->num_planets);
    printf("Maximum trash: %d\n", config->max_trash);
    printf("Initial trash: %d\n", config->initial_trash);
    printf("Ship capacity: %d\n", config->ship_capacity);
    printf("==============================\n");
}