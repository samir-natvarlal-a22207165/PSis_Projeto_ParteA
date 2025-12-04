#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "universe-data.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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
    universe->max_ships = config->max_ships; 
    universe->num_planets = 0;
    universe->num_trash = 0;
    universe->num_ships = 0;
    universe->recycling_planet_index = 0;
    universe->ship_capacity = config->ship_capacity;

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
    universe->ships = (ship_structure*)malloc(sizeof(ship_structure) * config->max_ships);
    if (!universe->ships) {
        fprintf(stderr, "Failed to allocate ships array\n");
        free(universe->planets);
        free(universe->trash);
        free(universe);
        return NULL;
    }

    // Initialize trash as inactive
    for (int i = 0; i < config->max_trash; i++) {
        universe->trash[i].active = false;
    }

    for (int i = 0; i < config->max_ships; i++) {
        universe->ships[i].trash_indexs = NULL;
        universe->ships[i].num_trash = 0;
    }

    printf("Universe created: %dx%d, max %d planets, max %d trash, max %d ships\n",
           universe->universe_width, universe->universe_height,
           universe->max_planets, universe->max_trash, universe->max_ships);

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

    if (universe->ships) {
        for (int i = 0; i < universe->num_ships; i++) {
            if (universe->ships[i].trash_indexs) {
                free(universe->ships[i].trash_indexs);
            }
        }
        free(universe->ships);
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
    planet->radius = (int)20;
    planet->mass = PLANET_MASS;
    planet->name = name;
    planet->is_recycling = false;
    planet->num_trash = 0;


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

//used for universe-simulator
void universe_initialize_planets(universe_data *universe) {
    if (!universe) return;

    printf("\nInitializing %d planets...\n", universe->max_planets);

    // Strategy: distribute planets in a pattern to avoid collisions
    // We'll use different strategies based on number of planets

    if (universe->max_planets == 1) {
        // Single planet in center
        universe_add_planet(universe, 
                          universe->universe_width / 2.0,
                          universe->universe_height / 2.0,
                          'A');
    } 
    else if (universe->max_planets == 2) {
        // Two planets horizontally spaced
        universe_add_planet(universe, 
                          universe->universe_width * 0.33,
                          universe->universe_height / 2.0,
                          'A');
        universe_add_planet(universe, 
                          universe->universe_width * 0.67,
                          universe->universe_height / 2.0,
                          'B');
    }
    else if (universe->max_planets <= 5) {
        // Arrange planets in a pentagon/circle pattern
        float cx = universe->universe_width / 2.0;
        float cy = universe->universe_height / 2.0;
        float radius = fmin(universe->universe_width, universe->universe_height) * 0.35;
        
        for (int i = 0; i < universe->max_planets; i++) {
            float angle = (2.0 * M_PI * i) / universe->max_planets - M_PI / 2.0; // Start from top
            float x = cx + radius * cos(angle);
            float y = cy + radius * sin(angle);
            
            universe_add_planet(universe, x, y, 'A' + i);
        }
    }
    else {
        // For more planets, use a grid-like distribution with some randomness
        int cols = (int)ceil(sqrt(universe->max_planets));
        int rows = (int)ceil((float)universe->max_planets / cols);
        
        float spacing_x = universe->universe_width / (cols + 1.0);
        float spacing_y = universe->universe_height / (rows + 1.0);
        
        // Add some margin from edges
        float margin_x = PLANET_RADIUS * 3;
        float margin_y = PLANET_RADIUS * 3;
        
        for (int i = 0; i < universe->max_planets; i++) {
            int row = i / cols;
            int col = i % cols;
            
            float base_x = margin_x + spacing_x * (col + 1);
            float base_y = margin_y + spacing_y * (row + 1);
            
            // Add small random offset to make it less uniform (±10% of spacing)
            float offset_x = (rand() % 100 - 50) / 500.0 * spacing_x;
            float offset_y = (rand() % 100 - 50) / 500.0 * spacing_y;
            
            float x = base_x + offset_x;
            float y = base_y + offset_y;
            
            // Ensure planets stay within bounds
            if (x < margin_x) x = margin_x;
            if (x > universe->universe_width - margin_x) x = universe->universe_width - margin_x;
            if (y < margin_y) y = margin_y;
            if (y > universe->universe_height - margin_y) y = universe->universe_height - margin_y;
            
            universe_add_planet(universe, x, y, 'A' + i);
        }
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
    trash->radius = (int)4;
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

// ===== Ship Functions =====
int universe_add_ship(universe_data *universe, float x, float y, char name){
    if (!universe) return -1;

    if (universe->num_ships >= universe->max_ships) {
        fprintf(stderr, "Cannot add ship: maximum reached\n");
        return -2;
    }

    int index = universe->num_ships;
    ship_structure *ship = &universe->ships[index];

    ship->trash_indexs = NULL; 
    ship->x = x;
    ship->y = y;
    ship->radius = SHIP_RADIUS;
    ship->name = name;
    ship->num_trash = 0;
    


    universe->num_ships++;

    printf("Added ship '%c' at (%.1f, %.1f)\n", name, x, y);

    return index;
}



ship_structure* universe_get_ship(universe_data *universe, int index) {
    if (!universe || index < 0 || index >= universe->max_ships) {
        return NULL;
    }
    /*
    if (!universe->ships[index].active) {
        return NULL;
    }
    */
    return &universe->ships[index];
}


// ===== Utility Functions =====

void correct_position(float *pos, int universe_size) {
    if (*pos < 0) {
        *pos += universe_size;
    } else if (*pos >= universe_size) {
        *pos -= universe_size;
    }
}

void chose_position(universe_data *universe, float *x, float *y,
                     int radius, int universe_width, int universe_height)
{
    bool valid = false;

    while (!valid) {
        valid = true;

        *x = rand() % universe_width;
        *y = rand() % universe_height;

        // Check planets
        for (int i = 0; i < universe->num_planets; i++) {
            planet_structure *planet = universe_get_planet(universe, i);
            if (do_circles_intersect(*x, *y, radius,
                                     planet->x, planet->y, planet->radius)) {
                valid = false;
                break;
            }
        }
        if (!valid) continue;

        // Check trash
        for (int i = 0; i < universe->num_trash; i++) {
            trash_structure *trash = universe_get_trash(universe, i);
            if (do_circles_intersect(*x, *y, radius,
                                     trash->x, trash->y, trash->radius)) {
                valid = false;
                break;
            }
        }
        for (int i = 0; i < universe->num_ships; i++) {
            ship_structure *ship = universe_get_ship(universe, i);
            if (do_circles_intersect(*x, *y, radius,
                                     ship->x, ship->y, ship->radius)) {
                valid = false;
                break;
            }
        }
    }
}



void check_colision_ship(universe_data *universe, int index, float *x, float *y,
                      int universe_width, int universe_height)
{
    ship_structure * ship = universe_get_ship(universe, index);
    if (!ship) {
        return;  // Nave inválida
    }
    
    // Check collision with other ships (skip self)
    for (int i = 0; i < universe->num_ships; i++) {
        if (i == index) continue;  // Pular a própria nave
        
        ship_structure *ship1 = universe_get_ship(universe, i);
        if (!ship1) continue;  // Pular se nave for NULL
        
        if (do_circles_intersect(*x, *y, ship->radius,
                                    ship1->x, ship1->y, ship1->radius)) {
            printf("Ship %c hit Ship %c\n", ship->name, ship1->name);
            return;
        }
    }
    
    // Check planets
    for (int i = 0; i < universe->num_planets; i++) {
        planet_structure *planet = universe_get_planet(universe, i);
        if (!planet) continue;  // Pular se planeta for NULL
        
        if (do_circles_intersect(*x, *y, ship->radius,
                                    planet->x, planet->y, planet->radius)) {
            if (planet->is_recycling){

                planet->num_trash += ship->num_trash;
                printf("Ship %c hit Recycling Planet %c which collected %d trash\n ", ship->name, planet->name, planet->num_trash); 
            
                if (ship->trash_indexs) {
                    free(ship->trash_indexs);
                }
                ship->trash_indexs = NULL;
                ship->num_trash = 0;
                break;


            }else{
                printf("Ship %c hit Planet %c\n", ship->name, planet->name);
                
                // Liberar trash coletado pela nave
                for (int j = 0; j < ship->num_trash; j++){
                    int trash_idx = ship->trash_indexs[j];
                    trash_structure * trash_released = universe_get_trash(universe, trash_idx);
                    if (trash_released) {
                        float new_x, new_y;
                        chose_position(universe, &new_x, &new_y, trash_released->radius, 
                                     universe_width, universe_height);
                        trash_released->x = new_x;
                        trash_released->y = new_y;
                        trash_released->active = true;
                    }
                }   
                
                if (ship->trash_indexs) {
                    free(ship->trash_indexs);
                }
                ship->trash_indexs = NULL;
                ship->num_trash = 0;
                return;
            }
        }
    }

    // Check trash
    for (int i = 0; i < universe->max_trash; i++) {
        trash_structure *trash = universe_get_trash(universe, i);
        if (!trash) continue;  // Pular se trash for NULL ou inativo
        
        if (do_circles_intersect(*x, *y, ship->radius,
                                    trash->x, trash->y, trash->radius)) {

            if(ship->num_trash < universe->ship_capacity){
                ship->num_trash++;
                ship->trash_indexs = (int*)realloc(ship->trash_indexs, sizeof(int) * ship->num_trash);
                ship->trash_indexs[ship->num_trash - 1] = i;
                trash->active = false;
                universe->num_trash--;  // Decrementar contador de trash ativo
            }
            break;
        }
    }
    
    ship->x = *x;
    ship->y = *y;
}


bool do_circles_intersect(float x1, float y1, float r1,
                          float x2, float y2, float r2)
{
    int dx = (int)(x2 - x1);
    int dy = (int)(y2 - y1);

    int dist2 = dx*dx + dy*dy;
    int rsum = (int)(r1 + r2);

    return dist2 < rsum * rsum;   // true if intersect/overlap
}


float calculate_distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

bool universe_has_collapsed(universe_data *universe) {
    if (!universe) return false;
    return universe->num_trash >= universe->max_trash;
}