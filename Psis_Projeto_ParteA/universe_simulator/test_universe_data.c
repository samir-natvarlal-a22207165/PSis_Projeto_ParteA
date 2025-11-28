#include "universe-data.h"
#include "config.h"
#include <stdio.h>
#include <math.h>

void test_vector_math() {
    printf("\n=== Testing Vector Math ===\n");
    
    // Test make_vector
    vector v1 = make_vector(3.0, 4.0);
    printf("Vector (3, 4): amplitude=%.2f, angle=%.2f radians (%.1f degrees)\n", 
           v1.amplitude, v1.angle, v1.angle * 180.0 / M_PI);
    
    // Test add_vectors
    vector v2 = make_vector(1.0, 0.0);
    vector v3 = add_vectors(v1, v2);
    printf("Vector (3,4) + (1,0): amplitude=%.2f, angle=%.2f radians\n", 
           v3.amplitude, v3.angle);
}

void test_planets() {
    printf("\n=== Testing Planets ===\n");
    
    universe_config config = {
        .universe_width = 800,
        .universe_height = 600,
        .num_planets = 5,
        .max_trash = 50,
        .initial_trash = 10,
        .ship_capacity = 10
    };
    
    universe_data *universe = universe_create(&config);
    if (!universe) {
        printf("Failed to create universe\n");
        return;
    }
    
    // Test manual planet addition
    printf("\n--- Manual planet addition ---\n");
    universe_add_planet(universe, 400, 300, 'X');
    universe_add_planet(universe, 200, 150, 'Y');
    universe_add_planet(universe, 600, 450, 'Z');
    
    // Set recycling planet
    universe_set_recycling_planet(universe, 1);
    
    // Print planet info
    printf("\nManually added planets:\n");
    for (int i = 0; i < universe->num_planets; i++) {
        planet_structure *p = universe_get_planet(universe, i);
        printf("  Planet %c: (%.0f, %.0f) mass=%.1f recycling=%s\n",
               p->name, p->x, p->y, p->mass, p->is_recycling ? "YES" : "NO");
    }
    
    universe_destroy(universe);
}

void test_planet_initialization() {
    printf("\n=== Testing Automatic Planet Initialization ===\n");
    
    // Test with different numbers of planets
    int test_cases[] = {1, 2, 3, 5, 8, 12};
    
    for (int i = 0; i < 6; i++) {
        int num_planets = test_cases[i];
        printf("\n--- Testing with %d planets ---\n", num_planets);
        
        universe_config config = {
            .universe_width = 800,
            .universe_height = 600,
            .num_planets = num_planets,
            .max_trash = 50,
            .initial_trash = 10,
            .ship_capacity = 10
        };
        
        universe_data *universe = universe_create(&config);
        if (!universe) {
            printf("Failed to create universe\n");
            continue;
        }
        
        // Initialize planets automatically
        universe_initialize_planets(universe);
        
        // Print planet positions
        printf("Planets in universe:\n");
        for (int j = 0; j < universe->num_planets; j++) {
            planet_structure *p = universe_get_planet(universe, j);
            printf("  Planet %c: (%.0f, %.0f) recycling=%s\n",
                   p->name, p->x, p->y, p->is_recycling ? "YES" : "NO");
        }
        
        universe_destroy(universe);
    }
}

void test_trash() {
    printf("\n=== Testing Trash ===\n");
    
    universe_config config = {
        .universe_width = 800,
        .universe_height = 600,
        .num_planets = 3,
        .max_trash = 50,
        .initial_trash = 10,
        .ship_capacity = 10
    };
    
    universe_data *universe = universe_create(&config);
    if (!universe) {
        printf("Failed to create universe\n");
        return;
    }
    
    // Test manual trash addition
    printf("\n--- Manual trash addition ---\n");
    int t1 = universe_add_trash(universe, 100, 100, 5.0, 0.0);
    int t2 = universe_add_trash(universe, 200, 200, 3.0, M_PI / 4);
    int t3 = universe_add_trash(universe, 300, 300, 2.0, M_PI / 2);
    
    printf("Added %d trash pieces manually\n", universe_count_active_trash(universe));
    
    // Print trash info
    printf("\nManually added trash:\n");
    for (int i = 0; i < universe->max_trash; i++) {
        trash_structure *t = universe_get_trash(universe, i);
        if (t) {
            printf("  Trash %d: (%.0f, %.0f) velocity=%.1f angle=%.2f\n",
                   i, t->x, t->y, t->velocity.amplitude, t->velocity.angle);
        }
    }
    
    // Remove one trash
    printf("\nRemoving trash %d\n", t2);
    universe_remove_trash(universe, t2);
    printf("Active trash: %d\n", universe_count_active_trash(universe));
    
    universe_destroy(universe);
}

void test_trash_initialization() {
    printf("\n=== Testing Automatic Trash Initialization ===\n");
    
    universe_config config = {
        .universe_width = 800,
        .universe_height = 600,
        .num_planets = 5,
        .max_trash = 50,
        .initial_trash = 20,
        .ship_capacity = 10
    };
    
    universe_data *universe = universe_create(&config);
    if (!universe) {
        printf("Failed to create universe\n");
        return;
    }
    
    // Initialize trash automatically
    universe_initialize_trash(universe, config.initial_trash);
    
    // Print first 10 trash pieces
    printf("\nFirst 10 trash pieces:\n");
    int count = 0;
    for (int i = 0; i < universe->max_trash && count < 10; i++) {
        trash_structure *t = universe_get_trash(universe, i);
        if (t) {
            printf("  Trash %d: (%.0f, %.0f) vel=%.2f angle=%.2f rad\n",
                   i, t->x, t->y, t->velocity.amplitude, t->velocity.angle);
            count++;
        }
    }
    
    universe_destroy(universe);
}

void test_utilities() {
    printf("\n=== Testing Utilities ===\n");
    
    // Test distance calculation
    float dist = calculate_distance(0, 0, 3, 4);
    printf("Distance from (0,0) to (3,4): %.2f\n", dist);
    
    // Test position correction (wraparound)
    float pos1 = -10.0;
    correct_position(&pos1, 800);
    printf("Position -10 corrected to: %.0f (in 800px universe)\n", pos1);
    
    float pos2 = 850.0;
    correct_position(&pos2, 800);
    printf("Position 850 corrected to: %.0f (in 800px universe)\n", pos2);
    
    float pos3 = 400.0;
    correct_position(&pos3, 800);
    printf("Position 400 stays at: %.0f (in 800px universe)\n", pos3);
}

int main() {
    printf("=== Universe Data Structure Tests ===\n");
    
    test_vector_math();
    test_planets();
    test_planet_initialization();
    test_trash();
    test_trash_initialization();
    test_utilities();
    
    printf("\n=== All tests completed ===\n");
    return 0;
}