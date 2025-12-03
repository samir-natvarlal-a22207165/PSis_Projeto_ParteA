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
        fd = create_client_channel(argv[1]);
    }else{
        fd = create_client_channel("172.29.160.1");
    }

    srand(time(NULL));
    char ch = 'a' + (rand() % 26);

    send_connection_message(fd, ch);
    char message[100];
    receive_response(fd, message);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_Window* win = SDL_CreateWindow("Client",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       800, 600, 0);

    SDL_Renderer * rend = render_window(win);
    
    int pos_x = 400;
    int pos_y = 300;
    
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
                                break;
                            case SDLK_RIGHT:
                                direction = RIGHT;
                                break;
                            case SDLK_UP:
                                direction = UP;
                                break;
                            case SDLK_DOWN:
                                direction = DOWN;
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
                            send_movement_message(fd, ch, direction);
                            receive_response(fd, message);
                            
                            if (strcmp(message, "WALL") == 0) {
                                printf("You hit a wall!\n");
                            }
                        }
                    }
                    break;
            }
        }

        // Render
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
        SDL_Rect dst = { pos_x, pos_y, 50, 50 };
        SDL_RenderFillRect(rend, &dst);

        SDL_RenderPresent(rend);
        
        SDL_Delay(16);  // ~60 FPS
    }
    
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}