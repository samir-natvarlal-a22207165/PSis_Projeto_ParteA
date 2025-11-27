#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int universe_width;
    int universe_height;
    int num_planets;
    int max_trash;
    int initial_trash;
    int ship_capacity;
} universe_config;

// Function to load configuration from file
// Returns 0 on success, -1 on error
int load_config(const char *filename, universe_config *config);

// Function to print configuration (for debugging)
void print_config(universe_config *config);

#endif // CONFIG_H