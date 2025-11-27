#include "universe-data.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// ===== Universe Management =====

universe_data* universe_create(universe_config *config) {
    universe_data *universe = (universe_data*)malloc(sizeof(universe_data));
    if (!universe) {
        fprintf(stderr, "Failed to allocate universe\n");
        return NULL;
    }

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