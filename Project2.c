#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <stdlib.h>

#define WIDTH 320
#define HEIGHT 200

/* DOOM fire state variable */
#define FIRE_W WIDTH
#define FIRE_H HEIGHT
#define FIRE_PALETTE_SIZE 37

static uint8_t fire_pixels[FIRE_H * FIRE_W];

/* Classic 37-color fire palette*/
static const uint32_t fire_palette[FIRE_PALETTE_SIZE] = {
    0xFF070707, 0xFF1F0707, 0xFF2F0F07, 0xFF470F07,
    0xFF571707, 0xFF671F07, 0xFF771F07, 0xFF8F2707,
    0xFF9F2F07, 0xFFA72707, 0xFFB72F07, 0xFFC73707,
    0xFFD73F07, 0xFFE74707, 0xFFF74F07, 0xFFFF5707,
    0xFFFF5F07, 0xFFFF6707, 0xFFFF6F07, 0xFFFF7707,
    0xFFFF7F07, 0xFFFF8707, 0xFFFF8F07, 0xFFFF9707,
    0xFFFF9F07, 0xFFFFA707, 0xFFFFAF07, 0xFFFFB707,
    0xFFFFBF07, 0xFFFFC707, 0xFFFFCF07, 0xFFFFD707,
    0xFFFFDF07, 0xFFFFE707, 0xFFFFEF07, 0xFFFFFF07
};

uint32_t framebuffer[WIDTH * HEIGHT];

void put_pixel(int x, int y, uint32_t color){
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT){
        return;
    }
}

void clear(uint32_t color){
    for(int i = 0; i < WIDTH * HEIGHT; i++){
        framebuffer[i] = color;
    }
}
int main() {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;

    const double target_frame = 1.0 / 60.0; // 0.0166666667 seconds per frame

    if(!SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("SDL Framebuffer", 
     WIDTH * 4, 
     HEIGHT * 4, 
     0
    );

    if(window == NULL){
        fprintf(stderr, "SDL_CreateWindow failed: %s \n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }


    renderer = SDL_CreateRenderer(  
        window, 
        NULL
    );

    if(renderer == NULL){
        fprintf(stderr, "SDL_CreateRenderer failed: %s \n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH,
        HEIGHT
    );

    if(texture == NULL){
        fprintf(stderr,"SDL_CreateTexture failed: %s \n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    

    


    uint8_t is_running = 1;
    uint32_t frame = 0;

    /**** Fire init ****/
    for(int i = 0; i < FIRE_H * FIRE_W; i++){
        fire_pixels[i] = 0;
    }
    for(int i = 0; i < FIRE_W; i++){
        fire_pixels[(FIRE_H - 1) * FIRE_W + i] = FIRE_PALETTE_SIZE - 1; 
    }
    

    while(is_running){


        uint64_t start = SDL_GetPerformanceCounter();
    

        /*Pull the events*/
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                is_running = 0;
            }
        }

        clear(0x2A2A2A); //Clear the framebuffer to white

        /*Here we manipulate our framebuffer array as we wish*/
        
        /* Fire effect update */
        for(uint32_t x = 0; x < FIRE_W; x++){
            for(uint32_t y = 1; y < FIRE_H; y++){
                uint32_t src = FIRE_W * y + x;
                uint32_t rand_idx = rand() % 4;
                int32_t dst = (int32_t)src - FIRE_W;
                int32_t dst_x = (int32_t)(src % FIRE_W) - (int32_t)rand_idx + 1;
                if(dst < 0 || dst_x < 0 || dst_x >= FIRE_W){
                    continue;
                }
                else{                                                                                                                       
                    dst = dst - (int32_t)(src % FIRE_W) + dst_x;
                    uint8_t src_val = fire_pixels[src];
                    uint8_t decay = rand_idx & 1;
                    fire_pixels[dst] = (src_val > decay) ? (src_val - decay) : 0;
                }
        }
    }
        for(int y = 0; y < HEIGHT; y++){
            for(int x = 0; x < WIDTH; x++){
                uint8_t intensity = fire_pixels[y * WIDTH + x];
                if(intensity >= FIRE_PALETTE_SIZE) intensity = FIRE_PALETTE_SIZE - 1;
                framebuffer[y * WIDTH + x] = fire_palette[intensity];
            }
        }

        SDL_UpdateTexture(texture,
        NULL,
        framebuffer,
        WIDTH * sizeof(uint32_t) //pitch --> the width of the image/texture in bytes, which is the width of the image multiplied by the size of each pixel in bytes
        );


        /*Display the window and the renderer */
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    

    uint64_t end = SDL_GetPerformanceCounter();

    double elapsed = (double)(end - start) / SDL_GetPerformanceFrequency();

    
    //Cap the framerate to 60 FPS
    if(elapsed < target_frame){
        SDL_Delay((target_frame - elapsed) * 1000.0);
    }

    frame += 1;

    



    

}
}