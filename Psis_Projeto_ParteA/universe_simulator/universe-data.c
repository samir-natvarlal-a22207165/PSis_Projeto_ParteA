#include "universe-data.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

// ===== Universe Management =====

universe_data* universe_create(universe_config *config) {
    universe_data *universe = (universe_data*)malloc(sizeof(universe_data));
    if (!universe) {
        fprintf(stderr, "Failed to allocate universe\n");
        return NULL;
    }

    // Initialize random seed (for planet positioning and trash generation)
    srand(time(NULL));

    // Initialize basic properties
    universe->universe_width = config->universe_width;
    universe->universe_height = config->universe_height;
    universe->max_planets = config->num_planets;
    universe->max_trash = config->max_trash;
    universe->num_planets = 0;
    universe->num_trash = 0;
    universe->recycling_planet_index = 0;

    // Allocate planets array
    universe->planets = (planet_structure*)malloc(sizeof(planet_structure) * config->num_planets);
    if (!universe->planets) {
        fprintf(stderr, "Failed to allocate planets array\n");
        free(universe);
        return NULL;
    }

    // Allocate trash array
    universe->trash = (trash_structure*)malloc(sizeof(trash_structure) * config->max_trash);
    if (!universe->trash) {
        fprintf(stderr, "Failed to allocate trash array\n");
        free(universe->planets);
        free(universe);
        return NULL;
    }

    // Initialize trash as inactive
    for (int i = 0; i < config->max_trash; i++) {
        universe->trash[i].active = false;
    }

    printf("Universe created: %dx%d, max %d planets, max %d trash\n",
           universe->universe_width, universe->universe_height,
           universe->max_planets, universe->max_trash);

    return universe;
}

void universe_destroy(universe_data *universe) {
    if (!universe) return;

    if (universe->planets) {
        free(universe->planets);
    }

    if (universe->trash) {
        free(universe->trash);
    }

    free(universe);
    printf("Universe destroyed\n");
}

// ===== Planet Functions =====

int universe_add_planet(universe_data *universe, float x, float y, char name) {
    if (!universe) return -1;

    if (universe->num_planets >= universe->max_planets) {
        fprintf(stderr, "Cannot add planet: maximum reached\n");
        return -1;
    }

    int index = universe->num_planets;
    planet_structure *planet = &universe->planets[index];

    planet->x = x;
    planet->y = y;
    planet->mass = PLANET_MASS;
    planet->name = name;
    planet->is_recycling = false;

    universe->num_planets++;

    printf("Added planet '%c' at (%.1f, %.1f)\n", name, x, y);

    return index;
}

planet_structure* universe_get_planet(universe_data *universe, int index) {
    if (!universe || index < 0 || index >= universe->num_planets) {
        return NULL;
    }
    return &universe->planets[index];
}

void universe_set_recycling_planet(universe_data *universe, int index) {
    if (!universe || index < 0 || index >= universe->num_planets) {
        return;
    }

    // Unset previous recycling planet
    if (universe->recycling_planet_index >= 0 && 
        universe->recycling_planet_index < universe->num_planets) {
        universe->planets[universe->recycling_planet_index].is_recycling = false;
    }

    // Set new recycling planet
    universe->recycling_planet_index = index;
    universe->planets[index].is_recycling = true;

    printf("Planet '%c' is now the recycling planet\n", 
           universe->planets[index].name);
}

void universe_initialize_planets(universe_data *universe) {
    if (!universe) return;

    printf("\nInitializing %d planets...\n", universe->max_planets);

    // Minimum distance between planets = 2 * radius + margin
    float min_distance = PLANET_RADIUS * 4.0; // 80 pixels (2 * 20 * 2)
    
    // Margin from edges
    float margin = PLANET_RADIUS * 2; // 40 pixels

    int max_attempts = 1000; // Maximum attempts to place a planet

    for (int i = 0; i < universe->max_planets; i++) {
        float x, y;
        bool valid_position = false;
        int attempts = 0;

        while (!valid_position && attempts < max_attempts) {
            attempts++;

            // Generate random position within bounds (with margin)
            x = margin + (rand() % (int)(universe->universe_width - 2 * margin));
            y = margin + (rand() % (int)(universe->universe_height - 2 * margin));

            // Check distance from all previously placed planets
            valid_position = true;
            for (int j = 0; j < i; j++) {
                float dx = x - universe->planets[j].x;
                float dy = y - universe->planets[j].y;
                float distance = sqrt(dx * dx + dy * dy);

                if (distance < min_distance) {
                    valid_position = false;
                    break;
                }
            }
        }

        if (!valid_position) {
            fprintf(stderr, "Warning: Could not find valid position for planet %d after %d attempts\n", 
                    i, max_attempts);
            // Place it anyway with some random position (fallback)
            x = margin + (rand() % (int)(universe->universe_width - 2 * margin));
            y = margin + (rand() % (int)(universe->universe_height - 2 * margin));
        }

        // Add planet with letter name
        char name = 'A' + (i % 26);
        universe_add_planet(universe, x, y, name);
    }

    // Set first planet as recycling planet by default
    if (universe->num_planets > 0) {
        universe_set_recycling_planet(universe, 0);
    }

    printf("Planets initialized successfully!\n");
}

// ===== Trash Functions =====

int universe_add_trash(universe_data *universe, float x, float y, 
                       float velocity_amplitude, float velocity_angle) {
    if (!universe) return -1;

    // Find first inactive trash slot
    int index = -1;
    for (int i = 0; i < universe->max_trash; i++) {
        if (!universe->trash[i].active) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        fprintf(stderr, "Cannot add trash: maximum reached\n");
        return -1;
    }

    trash_structure *trash = &universe->trash[index];

    trash->x = x;
    trash->y = y;
    trash->mass = TRASH_MASS;
    trash->velocity.amplitude = velocity_amplitude;
    trash->velocity.angle = velocity_angle;
    trash->acceleration.amplitude = 0.0;
    trash->acceleration.angle = 0.0;
    trash->active = true;

    universe->num_trash++;

    return index;
}

trash_structure* universe_get_trash(universe_data *universe, int index) {
    if (!universe || index < 0 || index >= universe->max_trash) {
        return NULL;
    }
    if (!universe->trash[index].active) {
        return NULL;
    }
    return &universe->trash[index];
}

void universe_remove_trash(universe_data *universe, int index) {
    if (!universe || index < 0 || index >= universe->max_trash) {
        return;
    }

    if (universe->trash[index].active) {
        universe->trash[index].active = false;
        universe->num_trash--;
    }
}

int universe_count_active_trash(universe_data *universe) {
    if (!universe) return 0;
    return universe->num_trash;
}

void universe_initialize_trash(universe_data *universe, int num_trash) {
    if (!universe) return;

    // Limit to max_trash
    if (num_trash > universe->max_trash) {
        num_trash = universe->max_trash;
    }

    printf("\nInitializing %d trash pieces...\n", num_trash);

    for (int i = 0; i < num_trash; i++) {
        // Random position in universe
        float x = (float)(rand() % universe->universe_width);
        float y = (float)(rand() % universe->universe_height);

        // Random initial velocity (small values)
        // Amplitude between 0.5 and 3.0 pixels per time unit
        float velocity_amp = 0.5 + (rand() % 250) / 100.0;
        
        // Random angle (0 to 2π)
        float velocity_angle = (rand() % 360) * M_PI / 180.0;

        // Add trash to universe
        int index = universe_add_trash(universe, x, y, velocity_amp, velocity_angle);
        
        if (index == -1) {
            fprintf(stderr, "Warning: Could not add trash %d\n", i);
            break;
        }
    }

    printf("Trash initialized: %d active pieces\n", universe_count_active_trash(universe));
}

// ===== Vector Math Functions =====

vector make_vector(float x, float y) {
    vector v;
    v.amplitude = sqrt(x * x + y * y);
    v.angle = atan2(y, x);
    return v;
}

vector add_vectors(vector v1, vector v2) {
    // Convert to cartesian coordinates
    float x1 = v1.amplitude * cos(v1.angle);
    float y1 = v1.amplitude * sin(v1.angle);
    float x2 = v2.amplitude * cos(v2.angle);
    float y2 = v2.amplitude * sin(v2.angle);

    // Add components
    float x = x1 + x2;
    float y = y1 + y2;

    // Convert back to polar
    return make_vector(x, y);
}

// ===== Utility Functions =====

void correct_position(float *pos, int universe_size) {
    if (*pos < 0) {
        *pos += universe_size;
    } else if (*pos >= universe_size) {
        *pos -= universe_size;
    }
}

float calculate_distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

void universe_print_info(universe_data *universe) {
    if (!universe) {
        printf("Universe is NULL\n");
        return;
    }

    printf("\n=== Universe Information ===\n");
    printf("Dimensions: %dx%d\n", universe->universe_width, universe->universe_height);
    printf("Planets: %d/%d\n", universe->num_planets, universe->max_planets);
    printf("Active trash: %d/%d\n", universe->num_trash, universe->max_trash);
    printf("\nPlanets:\n");
    
    for (int i = 0; i < universe->num_planets; i++) {
        planet_structure *p = &universe->planets[i];
        printf("  [%d] Planet '%c': position=(%.1f, %.1f) mass=%.1f %s\n",
               i, p->name, p->x, p->y, p->mass,
               p->is_recycling ? "[RECYCLING]" : "");
    }
    
    int active_trash = 0;
    for (int i = 0; i < universe->max_trash; i++) {
        if (universe->trash[i].active) {
            active_trash++;
        }
    }
    
    if (active_trash > 0) {
        printf("\nActive trash pieces: %d\n", active_trash);
    }
    
    printf("===========================\n\n");
}

bool universe_has_collapsed(universe_data *universe) {
    if (!universe) return false;
    
    // Universe collapses when trash reaches maximum
    return universe->num_trash >= universe->max_trash;
}