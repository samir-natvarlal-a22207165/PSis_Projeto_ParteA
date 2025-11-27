#include "config.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    universe_config config;
    const char *config_file = "universe.conf";

    // Allow custom config file via command line
    if (argc > 1) {
        config_file = argv[1];
    }

    printf("Loading configuration from: %s\n", config_file);

    if (load_config(config_file, &config) != 0) {
        fprintf(stderr, "Failed to load configuration. Exiting.\n");
        return 1;
    }

    printf("Configuration loaded successfully!\n\n");
    print_config(&config);

    return 0;
}