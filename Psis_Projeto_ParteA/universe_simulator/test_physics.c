#include "universe-data.h"
#include "physics-rules.h"
#include "config.h"
#include <stdio.h>
#include <math.h>

void test_simple_physics() {
    printf("\n=== Testing Simple Physics ===\n");
    
    universe_config config = {
        .universe_width = 800,
        .universe_height = 600,
        .num_planets = 1,
        .max_trash = 10,
        .initial_trash = 1,
        .ship_capacity = 10
    };
    
    universe_data *universe = universe_create(&config);
    if (!universe) {
        printf("Failed to create universe\n");
        return;
    }
    
    // Add one planet in center
    universe_add_planet(universe, 400, 300, 'A');
    
    // Add one trash piece to the right of the planet
    universe_add_trash(universe, 500, 300, 0.0, 0.0); // stationary trash
    
    printf("Initial state:\n");
    printf("  Planet A at (400, 300)\n");
    printf("  Trash at (%.2f, %.2f), velocity=(%.2f, %.2f rad)\n",
           universe->trash[0].x, universe->trash[0].y,
           universe->trash[0].velocity.amplitude, universe->trash[0].velocity.angle);
    
    // Simulate 10 time steps
    printf("\nSimulating 10 time steps:\n");
    for (int step = 0; step < 10; step++) {
        update_physics(universe);
        
        if (step < 5 || step == 9) { // Print first 5 and last step
            printf("  Step %d: pos=(%.2f, %.2f) vel=(%.2f, %.2f rad) acc=(%.4f, %.2f rad)\n",
                   step + 1,
                   universe->trash[0].x, universe->trash[0].y,
                   universe->trash[0].velocity.amplitude, universe->trash[0].velocity.angle,
                   universe->trash[0].acceleration.amplitude, universe->trash[0].acceleration.angle);
        }
    }
    
    printf("\nTrash should be moving towards the planet (left, angle ≈ π)\n");
    
    universe_destroy(universe);
}

void test_orbital_motion() {
    printf("\n=== Testing Orbital-like Motion ===\n");
    
    universe_config config = {
        .universe_width = 800,
        .universe_height = 600,
        .num_planets = 1,
        .max_trash = 1,
        .initial_trash = 1,
        .ship_capacity = 10
    };
    
    universe_data *universe = universe_create(&config);
    if (!universe) {
        printf("Failed to create universe\n");
        return;
    }
    
    // Add planet in center
    universe_add_planet(universe, 400, 300, 'A');
    
    // Add trash with perpendicular velocity (attempt circular orbit)
    // Distance 100px, velocity for circular orbit ≈ sqrt(GM/r) = sqrt(10/100) ≈ 0.316
    universe_add_trash(universe, 400, 200, 0.5, 0.0); // moving right
    
    printf("Initial state:\n");
    printf("  Planet A at (400, 300)\n");
    printf("  Trash at (%.2f, %.2f), velocity=(%.2f, %.2f rad)\n",
           universe->trash[0].x, universe->trash[0].y,
           universe->trash[0].velocity.amplitude, universe->trash[0].velocity.angle);
    
    // Track position over time
    printf("\nPosition over 50 time steps:\n");
    for (int step = 0; step < 50; step++) {
        update_physics(universe);
        
        if (step % 10 == 0) {
            float dist = calculate_distance(universe->trash[0].x, universe->trash[0].y, 400, 300);
            printf("  Step %2d: pos=(%.1f, %.1f) dist from planet=%.1f\n",
                   step + 1,
                   universe->trash[0].x, universe->trash[0].y,
                   dist);
        }
    }
    
    universe_destroy(universe);
}

void test_wraparound() {
    printf("\n=== Testing Wraparound (Teleportation) ===\n");
    
    universe_config config = {
        .universe_width = 800,
        .universe_height = 600,
        .num_planets = 0,
        .max_trash = 1,
        .initial_trash = 1,
        .ship_capacity = 10
    };
    
    universe_data *universe = universe_create(&config);
    if (!universe) {
        printf("Failed to create universe\n");
        return;
    }
    
    // Add trash near right edge, moving right
    universe_add_trash(universe, 750, 300, 10.0, 0.0); // fast rightward
    
    printf("Initial: pos=(%.1f, %.1f) moving right at speed 10.0\n",
           universe->trash[0].x, universe->trash[0].y);
    
    // Simulate until wraparound
    for (int step = 0; step < 10; step++) {
        float old_x = universe->trash[0].x;
        new_trash_position(universe);
        
        printf("  Step %d: pos=(%.1f, %.1f)", step + 1,
               universe->trash[0].x, universe->trash[0].y);
        
        if (universe->trash[0].x < old_x - 100) {
            printf(" <- WRAPAROUND!");
        }
        printf("\n");
    }
    
    universe_destroy(universe);
}

void test_friction() {
    printf("\n=== Testing Friction (1%% velocity reduction) ===\n");
    
    universe_config config = {
        .universe_width = 800,
        .universe_height = 600,
        .num_planets = 0,
        .max_trash = 1,
        .initial_trash = 1,
        .ship_capacity = 10
    };
    
    universe_data *universe = universe_create(&config);
    if (!universe) {
        printf("Failed to create universe\n");
        return;
    }
    
    // Add trash with initial velocity, no planets
    universe_add_trash(universe, 400, 300, 100.0, 0.0);
    
    printf("Initial velocity: %.2f\n", universe->trash[0].velocity.amplitude);
    printf("Expected: velocity * 0.99 each step\n\n");
    
    for (int step = 0; step < 100; step += 10) {
        for (int i = 0; i < 10; i++) {
            new_trash_velocity(universe);
        }
        printf("  After %3d steps: velocity = %.2f\n", step + 10, 
               universe->trash[0].velocity.amplitude);
    }
    
    printf("\nAfter 100 steps: %.2f (should be ≈ %.2f)\n", 
           universe->trash[0].velocity.amplitude,
           100.0 * pow(0.99, 100));
    
    universe_destroy(universe);
}

int main() {
    printf("=== Physics Rules Tests ===\n");
    
    test_simple_physics();
    test_orbital_motion();
    test_wraparound();
    test_friction();
    
    printf("\n=== All physics tests completed ===\n");
    return 0;
}