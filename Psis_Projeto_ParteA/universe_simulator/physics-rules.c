#include "physics-rules.h"
#include <math.h>
#include <stdio.h>

void new_trash_acceleration(universe_data *universe) {
    if (!universe) return;

    // For each piece of trash
    for (int n_trash = 0; n_trash < universe->max_trash; n_trash++) {
        if (!universe->trash[n_trash].active) continue;

        vector total_vector_force;
        total_vector_force.amplitude = 0;
        total_vector_force.angle = 0;

        // Calculate gravitational force from each planet
        for (int n_planet = 0; n_planet < universe->num_planets; n_planet++) {
            // Vector from trash to planet
            float force_vector_x = universe->planets[n_planet].x - universe->trash[n_trash].x;
            float force_vector_y = universe->planets[n_planet].y - universe->trash[n_trash].y;
            
            // Convert to polar vector
            vector local_vector_force = make_vector(force_vector_x, force_vector_y);
            
            // Calculate gravitational force magnitude
            // F = (G * M * m) / r²
            // Since G=1, M=10, m=1, and we want acceleration (F/m), this becomes: 10 / r²
            if (local_vector_force.amplitude > 0.1) { // Avoid division by very small numbers
                local_vector_force.amplitude = (universe->planets[n_planet].mass * universe->trash[n_trash].mass) /
                                               pow(local_vector_force.amplitude, 2);
            } else {
                local_vector_force.amplitude = 0;
            }
            
            // Add to total force
            total_vector_force = add_vectors(local_vector_force, total_vector_force);
        }

        // Set acceleration (F/m, but since m=1, acceleration = force)
        universe->trash[n_trash].acceleration = total_vector_force;
    }
}

void new_trash_velocity(universe_data *universe) {
    if (!universe) return;

    for (int n_trash = 0; n_trash < universe->max_trash; n_trash++) {
        if (!universe->trash[n_trash].active) continue;

        // Apply friction (reduces velocity by 1% per time unit)
        universe->trash[n_trash].velocity.amplitude *= TRASH_FRICTION; // 0.99

        // Add acceleration to velocity
        universe->trash[n_trash].velocity = add_vectors(
            universe->trash[n_trash].velocity,
            universe->trash[n_trash].acceleration
        );
    }
}

void new_trash_position(universe_data *universe) {
    if (!universe) return;

    for (int n_trash = 0; n_trash < universe->max_trash; n_trash++) {
        if (!universe->trash[n_trash].active) continue;

        // Update position based on velocity
        // x += velocity.amplitude * cos(velocity.angle)
        // y += velocity.amplitude * sin(velocity.angle)
        universe->trash[n_trash].x += universe->trash[n_trash].velocity.amplitude * 
                                      cos(universe->trash[n_trash].velocity.angle);
        universe->trash[n_trash].y += universe->trash[n_trash].velocity.amplitude * 
                                      sin(universe->trash[n_trash].velocity.angle);

        // Apply wraparound at universe boundaries
        correct_position(&universe->trash[n_trash].x, universe->universe_width);
        correct_position(&universe->trash[n_trash].y, universe->universe_height);
    }
}

void update_physics(universe_data *universe) {
    if (!universe) return;

    // Update physics in the correct order
    new_trash_acceleration(universe);
    new_trash_velocity(universe);
    new_trash_position(universe);
}