#ifndef PHYSICS_RULES_H
#define PHYSICS_RULES_H

#include "universe-data.h"

// Calculate new acceleration for all trash based on gravitational forces
// This implements the gravitational physics from the project specification
void new_trash_acceleration(universe_data *universe);

// Update velocity of all trash based on acceleration and friction
// Friction reduces velocity by 1% per time unit (0.99 factor)
void new_trash_velocity(universe_data *universe);

// Update position of all trash based on velocity
// Includes wraparound at universe boundaries
void new_trash_position(universe_data *universe);

// Update all physics (convenience function that calls all three above)
void update_physics(universe_data *universe);

#endif // PHYSICS_RULES_H