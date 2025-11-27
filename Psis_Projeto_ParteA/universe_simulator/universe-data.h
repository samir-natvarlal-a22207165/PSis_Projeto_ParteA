#ifndef UNIVERSE_DATA_H
#define UNIVERSE_DATA_H

#include <stdbool.h>
#include "config.h"

// Constants from project specification
#define PLANET_MASS 10.0
#define PLANET_RADIUS 20
#define TRASH_MASS 1.0
#define GRAVITATIONAL_CONSTANT 1.0
#define TRASH_FRICTION 0.99  // reduces velocity by 1% per time unit

// Vector structure for physics calculations
typedef struct {
    float amplitude;  // magnitude of the vector
    float angle;      // angle in radians
} vector;

// Planet structure
typedef struct {
    float x;              // X position
    float y;              // Y position
    float mass;           // mass (always 10.0)
    char name;            // single letter identifier (A, B, C, ...)
    bool is_recycling;    // true if this planet is the recycling planet
} planet_structure;

// Trash structure
typedef struct {
    float x;              // X position
    float y;              // Y position
    float mass;           // mass (always 1.0)
    vector velocity;      // velocity (amplitude + angle)
    vector acceleration;  // acceleration calculated by gravity
    bool active;          // true if this trash exists (not collected/destroyed)
} trash_structure;

// Universe structure - holds all universe data
typedef struct {
    planet_structure *planets;
    int num_planets;
    int max_planets;
    
    trash_structure *trash;
    int num_trash;
    int max_trash;
    
    int universe_width;
    int universe_height;
    
    int recycling_planet_index;  // index of current recycling planet
} universe_data;

// ===== Universe Management =====

// Create and initialize universe
universe_data* universe_create(universe_config *config);

// Destroy universe and free memory
void universe_destroy(universe_data *universe);

// ===== Planet Functions =====

// Add a planet to the universe at specified position
// Returns index of added planet, or -1 on error
int universe_add_planet(universe_data *universe, float x, float y, char name);

// Get planet by index
planet_structure* universe_get_planet(universe_data *universe, int index);

// Set recycling planet (by index)
void universe_set_recycling_planet(universe_data *universe, int index);

// ===== Trash Functions =====

// Add trash to the universe at specified position with initial velocity
// Returns index of added trash, or -1 on error
int universe_add_trash(universe_data *universe, float x, float y, 
                       float velocity_amplitude, float velocity_angle);

// Get trash by index
trash_structure* universe_get_trash(universe_data *universe, int index);

// Remove/deactivate trash
void universe_remove_trash(universe_data *universe, int index);

// Count active trash
int universe_count_active_trash(universe_data *universe);

// ===== Vector Math Functions =====

// Create a vector from x and y components
vector make_vector(float x, float y);

// Add two vectors
vector add_vectors(vector v1, vector v2);

// ===== Utility Functions =====

// Correct position for universe wraparound (teleportation at edges)
void correct_position(float *pos, int universe_size);

// Calculate distance between two points
float calculate_distance(float x1, float y1, float x2, float y2);

#endif // UNIVERSE_DATA_H