#include "Engine.hpp"

#define SCREEN_WIDTH 740
#define SCREEN_HEIGHT 580

int main(int argc, char** argv){
    // Window
    const char* title = "SHOOT NOW SDL VERSION";

    SDL_Surface* icon = IMG_Load("data/icon.png");

    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

    InitGame(renderer);

    // Game Default Scene
    Scene* main_scene;

    main_scene = new GamePlay();
    main_scene->Init(renderer);

    // Others...
    SDL_Keycode action_down, action_up;

    bool running = true;
    while(running){
        // Events
        SDL_Event event;

        while(SDL_PollEvent(&event)) {
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    main_scene->InputDown(event.key.keysym.sym);
                    break;

                case SDL_KEYUP:
                    main_scene->InputUp(event.key.keysym.sym);
                    break;
                default:
                    break;
            }
        }

        /* Update */
        main_scene->Update();

        /* Render */
        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
        SDL_RenderClear(renderer);

        main_scene->Render();
        
        // Debbuug
        SDL_RenderPresent(renderer);
        SDL_Delay(16.7);
    }

    // Finish
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    delete main_scene;

    EndGame();
    
    return 0;
}