#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "comm-fifo.h"
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include <time.h>





SDL_Renderer* render_window(SDL_Window* win){


    // triggers the program that controls
    // your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    SDL_Color backgroud_color;
    backgroud_color.r = 255;
    backgroud_color.g = 255;
    backgroud_color.b = 255;
    backgroud_color.a = 255;
    SDL_SetRenderDrawColor(rend, 
        backgroud_color.r, backgroud_color.g, backgroud_color.b, 
        backgroud_color.a);
    SDL_RenderClear(rend);
    SDL_RenderPresent(rend);
    return rend;
}
    


int main(int argc,  char** argv){
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
    receive_response (fd, message);

            // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        exit(-1);
    }

    SDL_Window* win = SDL_CreateWindow("Client", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       800, 600, 0);

    SDL_Renderer * rend = render_window(win);
    
    Position position; 			
    int close = 0;
    int mouse_down = 0;    

    while (!close) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: close = 1; break;
                case SDL_MOUSEBUTTONDOWN: mouse_down = 1; break;
                case SDL_MOUSEBUTTONUP: mouse_down = 0; break;
                case SDL_MOUSEMOTION:
                    if (mouse_down) {
                        position.x = event.motion.x;
                        position.y = event.motion.y;
                        send_movement_message(fd, ch, &position);
                    }
                    break;
            }
        }

        // Render
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_RenderClear(rend);

        SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
        SDL_Rect dst = { position.x, position.y, 50, 50 };
        SDL_RenderFillRect(rend, &dst);

        SDL_RenderPresent(rend);
    }
    SDL_DestroyRenderer(rend);

    // destroy window
    SDL_DestroyWindow(win);
    
    // close SDL
    SDL_Quit();		

	return 0;
}
    	