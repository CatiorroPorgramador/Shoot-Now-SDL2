#include "Engine.hpp"

#define SCREEN_WIDTH 740
#define SCREEN_HEIGHT 580

void Shoot(std::vector<Shot*> *shots, SDL_Rect* reference) {
    Shot* ns = new Shot(reference->x+80, reference->y+34);
    shots->push_back(ns);
}

int main(int argc, char** argv){
    // Window
    const char* title = "SHOOT NOW SDL VERSION";

    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_SetWindowResizable(window, SDL_TRUE);

    // Others...
    SDL_Keycode action_down, action_up;

    Uint8 timer_spawner, timer_shoot;
    timer_spawner = 0;
    timer_shoot = 0;

    // Groups
    std::vector<Zombie*> zombies;
    std::vector<Shot*> shots;

    // Guns
    Gun m4("M4", 30, 8, 18, 0);
    Gun ak47("Ak-47", 30, 7, 19, 1);
    Gun glock("Glock", 17, 20, 15, 4);

    // Objects
    Player *player = new Player();
    player->LoadTexture(renderer);


    bool running = true;
    while(running){
        // Events
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    action_down = event.key.keysym.sym;

                    switch (action_down)
                    {
                    case SDLK_a:
                        player->dx = -player->speed;
                        break;
                    case SDLK_d:
                        player->dx = player->speed;
                        break;
                    case SDLK_w:
                        player->dy = -player->speed;
                        break;
                    case SDLK_s:
                        player->dy = player->speed;
                        break;
                    case SDLK_SPACE:
                        if (!player->shooting) timer_shoot = 40;
                        player->shooting = true;
                        break;
                    case SDLK_1:
                        player->ChangeGun(m4);
                        break;
                    case SDLK_2:
                        player->ChangeGun(glock);
                        break;
                    default:
                        break;
                    }
                    
                    break;
                case SDL_KEYUP:
                    action_up = event.key.keysym.sym;

                    if (action_up == SDLK_a || action_up == SDLK_d)
                        player->dx = 0;
                    if (action_up == SDLK_s || action_up == SDLK_w)
                        player->dy = 0;
                    
                    if (action_up == SDLK_SPACE)
                        player->shooting = false;
                    break;

                default:
                    break;
            }
        }

        // Update
        timer_spawner++;

        // Player
        player->Update(action_down, action_up);

        if (timer_shoot > 8) {
            Shoot(&shots, player->rect);
            timer_shoot = 0;
        }

        if (player->shooting)
            timer_shoot++;

        for (int i{0}; i < zombies.size(); ++i) {
            zombies[i]->Update();

            if (SDL_HasIntersection(zombies[i]->rect, player->rect))
                zombies[i]->alive = false;

            if (!zombies[i]->alive) {
                delete zombies[i];
                zombies.erase(zombies.begin()+i);
            }
        }

        for (int i{0}; i < shots.size(); ++i) {
            shots[i]->Update();

            for (int z{0}; z < zombies.size(); ++z) {
                if (SDL_HasIntersection(shots[i]->rect, zombies[z]->rect)) {
                    shots[i]->alive = false;
                    zombies[z]->alive = false;
                }
            }

            if (!shots[i]->alive) {
                delete shots[i];
                shots.erase(shots.begin()+i);
            }
        }

        if (timer_spawner >= 60) {
            Zombie* nz = new Zombie();
            nz->LoadTexture(renderer);
            zombies.push_back(nz);
            
            timer_spawner = 0;
        }

        // Render Begin
        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
        SDL_RenderClear(renderer);

        for (auto zom : zombies) {
            zom->Render(renderer);
        }

        for (auto sht : shots) {
            sht->Render(renderer);
        }

        player->Render(renderer);

        // Debbuug
        printf("Number of Zombies: %d\r", zombies.size());

        SDL_RenderPresent(renderer);
        SDL_Delay(16.7);
    }

    // Finish
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    delete player;

    for (int i{0}; i < zombies.size(); ++i) {
        printf("Deleting remaining zombie... number(%d/%d)\r", i+1, zombies.size());
        delete zombies[i];
    }
    putchar('\n');

    for (int i{0}; i < shots.size(); ++i) {
        printf("Deleting remaining Shots... number(%d/%d)\r", i+1, shots.size());
        delete shots[i];
    }
    
    return 0;
}