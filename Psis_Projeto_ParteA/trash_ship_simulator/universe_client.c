#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "zmq-comm.h"
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include <time.h>

SDL_Renderer* render_window(SDL_Window* win){
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    SDL_Color backgroud_color = {255, 255, 255, 255};
    SDL_SetRenderDrawColor(rend, 
        backgroud_color.r, backgroud_color.g, backgroud_color.b, 
        backgroud_color.a);
    SDL_RenderClear(rend);
    SDL_RenderPresent(rend);
    return rend;
}

int main(int argc, char** argv){
    void * fd;
    if (argc >= 2){
        fd = create_client_channel(argv[1]); // ./universe_client 172.29.160.1
    }else{
        fd = create_client_channel("127.0.0.1");  // localhost para conexão local
    }

    srand(time(NULL));
    char ch = 'a' + (rand() % 26);

    send_connection_message(fd, ch);
    char message[100];
    receive_response(fd, message);
    if (strcmp(message, "OK") == 0) {
        printf("Connected to the Universe with ship %c!\n", ch);
        printf("\n");
        printf("╔════════════════════════════════════════════╗\n");
        printf("║  CLICK ON THE CLIENT WINDOW TO ACTIVATE!  ║\n");
        printf("║  Then use ARROW KEYS to move your ship    ║\n");
        printf("╚════════════════════════════════════════════╝\n");
        printf("\n");
    }else if((strcmp(message, "NOT OK") == 0)){
        printf("Ship %c already connected!\n", ch);
        exit(-1);
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_Window* win = SDL_CreateWindow("Client - Click to Control",
                                       520,  // Left of server (820 - 300 = 520)
                                       250,  // Centered vertically with server
                                       300, 300, 0);  // 300x300 pixels

    // Raise window to bring it to the front
    SDL_RaiseWindow(win);

    SDL_Renderer * rend = render_window(win);
    
    direction_t last_direction = -1;  // Nenhuma seta pressionada inicialmente
    int has_direction = 0;
    
    int close = 0;

    while (!close) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: 
                    close = 1; 
                    break;
                    
                case SDL_KEYDOWN:
                    {
                        direction_t direction;
                        int valid_key = 1;
                        
                        switch (event.key.keysym.sym) {
                            case SDLK_LEFT:
                                direction = LEFT;
                                printf("LEFT\n");
                                break;
                            case SDLK_RIGHT:
                                direction = RIGHT;
                                printf("RIGHT\n");
                                break;
                            case SDLK_UP:
                                direction = UP;
                                printf("UP\n");
                                break;
                            case SDLK_DOWN:
                                direction = DOWN;
                                printf("DOWN\n");
                                break;
                            case SDLK_ESCAPE:                                                       
                                close = 1;
                                valid_key = 0;
                                break;
                            default:
                                valid_key = 0;
                                break;
                        }
                        
                        if (valid_key && !close) {
                            // Always update the arrow direction, even if movement is blocked
                            last_direction = direction;
                            has_direction = 1;
                            
                            send_movement_message(fd, ch, direction);
                            receive_response(fd, message);
                            
                            if (strcmp(message, "BAD MOVEMENT") == 0) {
                                printf("You hit a something!\n");
                            }
                        }
                    }
                    break;
            }
        }

        // Render - clear to white background
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_RenderClear(rend);

        // Draw large black arrow occupying entire window
        if (has_direction) {
            // Set black color for all drawing operations
            SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
            
            // Draw a simple, large filled arrow using rectangles and triangles
            switch (last_direction) {
                case UP: {
                    // Triangle pointing up (top half)
                    for (int y = 0; y < 150; y++) {
                        int width = (y * 300) / 150;  // Width increases as we go down
                        int x_start = 150 - width/2;
                        int x_end = 150 + width/2;
                        SDL_RenderDrawLine(rend, x_start, y, x_end, y);
                    }
                    // Rectangle (bottom half - stem)
                    SDL_Rect stem = {100, 150, 100, 150};
                    SDL_RenderFillRect(rend, &stem);
                    break;
                }
                    
                case DOWN: {
                    // Rectangle (top half - stem)
                    SDL_Rect stem_down = {100, 0, 100, 150};
                    SDL_RenderFillRect(rend, &stem_down);
                    // Triangle pointing down (bottom half)
                    for (int y = 150; y < 300; y++) {
                        int width = ((300 - y) * 300) / 150;  // Width decreases as we go down
                        int x_start = 150 - width/2;
                        int x_end = 150 + width/2;
                        SDL_RenderDrawLine(rend, x_start, y, x_end, y);
                    }
                    break;
                }
                    
                case LEFT: {
                    // Triangle pointing left (left half)
                    for (int x = 0; x < 150; x++) {
                        int height = (x * 300) / 150;  // Height increases as we go right
                        int y_start = 150 - height/2;
                        int y_end = 150 + height/2;
                        for (int y = y_start; y <= y_end; y++) {
                            SDL_RenderDrawPoint(rend, x, y);
                        }
                    }
                    // Rectangle (right half - stem)
                    SDL_Rect stem_left = {150, 100, 150, 100};
                    SDL_RenderFillRect(rend, &stem_left);
                    break;
                }
                    
                case RIGHT: {
                    // Rectangle (left half - stem)
                    SDL_Rect stem_right = {0, 100, 150, 100};
                    SDL_RenderFillRect(rend, &stem_right);
                    // Triangle pointing right (right half)
                    for (int x = 150; x < 300; x++) {
                        int height = ((300 - x) * 300) / 150;  // Height decreases as we go right
                        int y_start = 150 - height/2;
                        int y_end = 150 + height/2;
                        for (int y = y_start; y <= y_end; y++) {
                            SDL_RenderDrawPoint(rend, x, y);
                        }
                    }
                    break;
                }
            }
        }

        SDL_RenderPresent(rend);
        
        SDL_Delay(16);  // ~60 FPS
    }
    
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}