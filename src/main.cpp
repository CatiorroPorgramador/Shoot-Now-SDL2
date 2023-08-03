#include "Engine.hpp"

#define SCREEN_WIDTH 740
#define SCREEN_HEIGHT 580

void Shoot(std::vector<Shot*> *shots, float speed, SDL_Rect* rect, Uint8 point_x, Uint8 point_y, Uint8 w, Uint8 h) {
    Shot* ns = new Shot(rect->x+point_x, rect->y+point_y, w, h);
    ns->speed = speed;
    shots->push_back(ns);
}

int main(int argc, char** argv){
    // Window
    const char* title = "SHOOT NOW SDL VERSION";

    SDL_Surface* icon = IMG_Load("data/icon.png");

    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);

    // GUI
    InitGame(renderer);
    
    GUI gui;
    gui.Update(renderer);

    // Others...
    SDL_Keycode action_down, action_up;

    Uint8 timer_spawner, timer_items, timer_shoot;
    timer_spawner = 0;
    timer_shoot = 0;
    timer_items = 0;

    // Groups
    std::vector<Zombie*> zombies;
    std::vector<Shot*> shots;
    std::vector<Item*> items;

    // Guns
    Gun paw("Paw", -1, 30, 7, 5, -40, -40);
    paw.SetBulletSize(30, 30);
    Gun m4("M4", 30, 7, 23, 0, 40, 0);
    m4.SetBulletSize(8, 4);
    m4.SetBulletPosition(80, 49);
    m4.SetDamage(30);

    Gun ak47("Ak-47", 30, 7, 23, 1, 20, 0);
    ak47.SetBulletSize(8, 4);
    ak47.SetBulletPosition(50, 49);
    
    Gun glock("Glock", 17, 40, 15, 2, 10, 0);
    glock.SetBulletSize(5, 3);
    glock.SetBulletPosition(60, 49);
    glock.SetDamage(15);

    Gun rev_n("Revolver", 6, 60, 13, 3, 10, 0);
    rev_n.SetBulletSize(6, 3);
    Gun rev_c("Canela Seca", 6, 60, 16, 4, 10, 0);
    rev_c.SetBulletSize(5, 3);

    Gun slot[2] = {glock, m4};
    uint8_t slot_index = 0;

    // Objects
    Player *player = new Player();
    player->LoadTexture(renderer);
    player->ChangeGun(slot[0]);

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

                    switch (action_down) {
                    case SDLK_a: player->dx = -player->speed;
                        break;
                    case SDLK_d: player->dx = player->speed;
                        break;
                    case SDLK_w: player->dy = -player->speed;
                        break;
                    case SDLK_s: player->dy = player->speed;
                        break;
                    case SDLK_SPACE:
                        if (!player->shooting) timer_shoot = player->gun_time_shoot;
                        player->shooting = true;
                        break;
                    case SDLK_1:
                        player->ChangeGun(slot[0]);
                        slot_index = 0;
                        break;
                    case SDLK_2:
                        player->ChangeGun(slot[1]);
                        slot_index = 1;
                        break;
                    default:
                        break;
                    }
                    break;

                case SDL_KEYUP:
                    action_up = event.key.keysym.sym;

                    if (action_up == SDLK_a || action_up == SDLK_d)     player->dx = 0;
                    if (action_up == SDLK_s || action_up == SDLK_w)     player->dy = 0;
                    if (action_up == SDLK_SPACE)                        player->shooting = false;
                    break;
                default:
                    break;
            }
        }

        /* Update */

        // Timers
        timer_spawner++;
        timer_items++;

        // Player
        player->Update(action_down, action_up);
        player_y = player->rect->y;

        if (player->shooting) timer_shoot++;

        if (timer_shoot > player->gun_time_shoot) {
            Shoot(&shots, player->gun_shot_speed, player->rect, player->point_x, player->point_y, player->shot_rect->w, player->shot_rect->h);
            timer_shoot = 0;
        }

        // Zombies
        for (int i{0}; i < zombies.size(); ++i) {
            zombies[i]->Update();

            if (SDL_HasIntersection(zombies[i]->rect, player->rect)) {
                zombies[i]->alive = false;
                gui.hp_val -= 20;
                gui.Update(renderer);
            }

            if (!zombies[i]->alive) {
                delete zombies[i];
                zombies.erase(zombies.begin()+i);
            }
        }

        // Shots
        for (int i{0}; i < shots.size(); ++i) {
            shots[i]->Update();

            for (int z{0}; z < zombies.size(); ++z) {
                if (SDL_HasIntersection(shots[i]->rect, zombies[z]->rect)) {
                    shots[i]->alive = false;
                    zombies[z]->Hit(slot[slot_index].damage);
                }
            }

            if (!shots[i]->alive) {
                delete shots[i];
                shots.erase(shots.begin()+i);
            }
        }

        // Items
        for (int i{0}; i < items.size(); ++i) {
            items[i]->Update();

            if (SDL_HasIntersection(items[i]->rect, player->rect)) {
                if (items[i]->frame == 0)
                    gui.coins_val += 5;
            
                else if (items[i]->frame == 1)
                    gui.hp_val += 20;
                items[i]->alive = false;
                
                gui.Update(renderer);
            }

            if (!items[i]->alive) {
                delete items[i];
                items.erase(items.begin()+i);
            }
        }

        // Timers Conditional
        if (timer_spawner >= 60) {
            Zombie* nz = new Zombie();
            zombies.push_back(nz);
            
            timer_spawner = 0;
        }

        if (timer_items >= 120) {
            Item* item = new Item();
            items.push_back(item);

            timer_items = 0;
        }

        /* Render */
        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
        SDL_RenderClear(renderer);

        for (auto zom : zombies) {
            zom->Render(renderer);
        }
        
        for (auto sht : shots) {
            sht->Render(renderer);
        }

        for (auto itm : items) {
            itm->Render(renderer);
        }

        player->Render(renderer);

        gui.Render(renderer);
        
        // Debbuug
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
    putchar('\n');

    for (int i{0}; i < items.size(); ++i) {
        printf("Deleting remaining items... number(%d/%d)\r", i+1, items.size());
        delete items[i];
    }
    putchar('\n');

    EndGame();
    
    return 0;
}