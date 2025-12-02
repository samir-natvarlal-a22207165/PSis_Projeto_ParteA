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
    float radius;         // radiys (always 20.0)
    char name;            // single letter identifier (A, B, C, ...)
    bool is_recycling;    // true if this planet is the recycling planet
} planet_structure;

// Trash structure
typedef struct {
    float x;              // X position
    float y;              // Y position
    float mass;           // mass (always 1.0)
    float radius;         // radiys (always 1.0)
    vector velocity;      // velocity (amplitude + angle)
    vector acceleration;  // acceleration calculated by gravity
    bool active;          // true if this trash exists (not collected/destroyed)
} trash_structure;

// Ship structure
typedef struct {
    float x;              // X position
    float y;              // Y position
    float mass;           // mass (always 10.0)
    float radius;         // radiys (always 10.0)
    char name;            // single letter identifier (A, B, C, ...)
    bool is_recycling;    // true if this planet is the recycling planet
} ship_structure;

// Universe structure - holds all universe data
typedef struct {
    planet_structure *planets;
    int num_planets;
    int max_planets;
    
    trash_structure *trash;
    int num_trash;
    int max_trash;

    ship_structure *ships;
    int num_ships;
    int max_ships;
    
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

// Initialize planets in the universe with automatic positioning
void universe_initialize_planets(universe_data *universe);

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

// Initialize trash in the universe with random positions and velocities
void universe_initialize_trash(universe_data *universe, int num_trash);

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

// Print universe information (for debugging)
void universe_print_info(universe_data *universe);

// Check if universe has collapsed (trash reached maximum)
bool universe_has_collapsed(universe_data *universe);

void chose_position(universe_data *universe,int *x, int *y, int radius, int universe_width, int universe_height);

void check_position_ships(universe_data *universe, int *x, int *y,
                     int radius, int universe_width, int universe_height);

bool do_circles_intersect(int x1, int y1, int radius1,  int x2, int y2, int radius2);

#endif // UNIVERSE_DATA