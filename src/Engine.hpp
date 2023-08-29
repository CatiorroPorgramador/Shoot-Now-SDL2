#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <functional>
#include <string>
#include <unordered_map>

enum ZOMBIES {
    NORMAL, BRICK, FUNK
};

enum CLOTHES {
    DOLMAN_BODY, DOLMAN_HEAD, JULIET
};

/* Textures */
std::vector<SDL_Texture*> clothes_textures;

std::vector<SDL_Texture*> zombie_textures;

SDL_Texture* zombie_funk_texture;
SDL_Texture* zombie_normal_texture;
SDL_Texture* zombie_brick_texture;
SDL_Texture* item_texture;

TTF_Font* font;

int player_y;

class Text {
public:
    Text(std::string text, uint_fast16_t x, uint_fast16_t y) {
        t = text;
        
        r = new SDL_Rect();
        r->x = x;
        r->y = y;
    }

    ~Text() {
        SDL_FreeSurface(s);
        SDL_DestroyTexture(txt);

        delete r;
    }

    void SetText(std::string text) {
        t = text;
        TTF_SizeText(font, t.c_str(), &r->w, &r->h);
    }

    void Update(SDL_Renderer* renderer) {
        s = TTF_RenderText_Solid(font, t.c_str(), {255, 255, 255});
        SDL_DestroyTexture(txt);
        txt = SDL_CreateTextureFromSurface(renderer, s);
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, txt, nullptr, r);
    }

private:
    SDL_Texture* txt;
    SDL_Surface* s;
    SDL_Rect *r;
    std::string t;
};

struct Gun {
public:
    Gun(const char* _name, Uint8 _max_bullets, Uint8 _delay_shoot, float _speed_shot, int _frame, Uint8 x, Uint8 y) {
        this->name = (char*) _name;
        this->delay_shoot = _delay_shoot;
        this->speed_shot = _speed_shot;
        this->frame = _frame;

        this->x = x;
        this->y = y;
        this->w = 8;
        this->h = 4;
    }

    void SetBulletSize(Uint8 w, Uint8 h) {
        this->w = w;
        this->h = h;
    }

    void SetBulletPosition(int x, int y) {
        this->point_x = x;
        this->point_y = y;
    }

    void SetDamage(int damage) {
        this->damage = damage;
    }

    char* name;
    int damage;
    Uint8 max_bullets, delay_shoot, delay_index, x, y, w, h, point_x, point_y; // Points are the shoots points start
    int frame;
    float speed_shot;
};
// Weapons
Gun m4("M4", 30, 7, 23, 0, 40, 0);
Gun glock("Glock", 17, 40, 15, 2, 10, 0);

struct AnimationManager {
public:
    AnimationManager(SDL_Rect* sheet) {
        this->s = sheet;
    }

    void Update() { 
        if (p) {
            i++;

            if (i > as) {
                finished = true;
                f = anim[fr];
                fr++;             // To get next point of vector Anim
                i = 0;

                s->x = f*jmp;
            }

            if (fr > anim.size()) { // Aniamtion Finished
                finished = this->name;
                p = loop; // if loop is true, continue animation 
                fr = 0;
                f = anim[fr];
                i = 0;

                s->x = f*jmp;
            }
        }
    }

    void Play(const char* name) {
        if (std::string(name) != this->name) {
            p = true;
            anim = anims[name];
            this->name = std::string(name);
        }
    }

    void Stop() {
        name = "";
        p = false;
        i = 0;
        fr = 0;
        f = 0;
    }

    void CreateAnimation(const char* name, std::vector<int_fast8_t> frames) {
        anims.insert(std::make_pair(name, frames));
    }

    void SetAnimationSpeed(int_fast16_t speed) {
        if (as != speed) as = speed;
    }

    std::string finished;
    bool loop = true;

    std::string name;

private:
    bool p = false; // Playing Some Animation
    int_fast16_t i = 0; // Animation Index
    int_fast16_t as = 30; // Animation Speed

    int_fast8_t f = 0;  // Animation Frame, Index Animation
    int_fast8_t fr = 0; // Reference Animatoin Frame
    int_fast8_t jmp = 16; // Size of unique sprite to jump
    std::vector<int_fast8_t> anim;

    std::unordered_map<const char*, std::vector<int_fast8_t>> anims;

    SDL_Rect* s; // Sheet
};

/* Zombies Updates: Differents Updates for differents zombies, to improve game fun */
std::vector<std::function<void(SDL_Rect*, AnimationManager*, int*, int*, int*)>> zom_upt;

std::vector<int_fast8_t> walk_animation = {0, 1, 2, 3, 4, 5, 6, 7, 8};

void NormalZombie(SDL_Rect* r, AnimationManager* a, int* dx, int* dy, int* state) {
    *dx = -1;
    a->Play("walk");

    a->Update();
}

void FunkZombie(SDL_Rect* r, AnimationManager* a, int* dx, int* dy, int* state) {
    if (*state != 2) {
        if (r->x <= 400 && *state != 1)
            *state = 1; 
        
        if (a->finished == "smoke") {
            *state = 2;
        }
    }
    
    if (*state == 1) {
        a->SetAnimationSpeed(15);
        a->Play("smoke");

        a->loop = false;
        *dx = 0;
    }
    

    else if (*state == 2) {
        a->SetAnimationSpeed(5);
        a->loop = true;
        a->Play("walk");
        r->x -= 2;

        if (r->y > player_y) r->y -= 2;
        else r->y += 2;
    } else if (*state == 3) {
        *dx = 1;
    }
    else {
        *dx = -1;
    }

    a->Update();
}

class Zombie {
public:
    Zombie() {
        rect = new SDL_Rect {740, (rand()%490), 90, 90};
        sheet_rect = new SDL_Rect {0, 0, 16, 16};

        alive = true;

        state = new int(0);
        dx = new int(0);
        dy = new int (0);

        animation = new AnimationManager(sheet_rect);
        animation->CreateAnimation("walk", walk_animation);
        animation->Play("walk");
        animation->SetAnimationSpeed(10);

        hp = 50;
        type = rand()%3;
        upt = zom_upt[type];

        if (type == ZOMBIES::FUNK) {
            hp = 60;
            animation->CreateAnimation("smoke", {9, 10, 11, 12, 13, 13, 13, 13, 14, 15, 16, 17, 13, 13, 13, 13, 14, 15, 16, 17});
        } else if (type == ZOMBIES::BRICK) {
            hp = 80;
        }
    }

    ~Zombie() {
        free(state);

        delete rect;
        delete sheet_rect;

        delete animation;
    }

    void Update() {
        upt(rect, animation, dx, dy, state);

        rect->x += *dx;
        rect->y += *dy;

        if (rect->x < -rect->w || hp <= 0) alive = false;

    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, zombie_textures[type], sheet_rect, rect);
    }

    void Hit(int damage, int shot_y) {
        hp -= damage;
    }

    bool alive;

    int* state;

    SDL_Rect *rect;
    SDL_Rect *sheet_rect;

private:
    Uint8 type;
    int hp;
    int* dx;
    int* dy;

    AnimationManager* animation;
    std::function<void(SDL_Rect*, AnimationManager*, int*, int*, int*)> upt;

    float speed;
};

class Player {
public:
    Player() {
        rect = new SDL_Rect {0, (580/2)-(16*4)/2, 16*5, 16*5};
        sheet_rect = new SDL_Rect {0, 0, 16, 16};
        gun_rect = new SDL_Rect {0, 0, 105, 105};
        gun_sheet_rect = new SDL_Rect {0, 0, 32, 32};
        shot_rect = new SDL_Rect {0, 0, 0, 0};

        shooting = false;
        can_shoot = true;

        dx = 0;
        dy = 0;
        speed = 3;
        frame = 2;

        d_gun[0] = 0;
        d_gun[1] = 0;

        clothes.push_back(DOLMAN_BODY);
        clothes.push_back(JULIET);

        printf("Player Has Created...\n");
    }

    ~Player() {
        SDL_DestroyTexture(texture);
        SDL_DestroyTexture(gun_texture);
        delete rect;
        delete sheet_rect;

        delete gun_rect;
        delete gun_sheet_rect;
        delete shot_rect;

        printf("Player Has Deleted...\n");
    }

    void LoadTexture(SDL_Renderer* renderer) {
        // Player
        SDL_Surface* surface = IMG_Load("data/players/player-spritesheet.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        
        // Guns
        surface = IMG_Load("data/guns-spritesheet.png");
        gun_texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);
    }

    void ChangeGun(Gun gun) {
        gun_sheet_rect->x = 32*gun.frame;
        gun_time_shoot = gun.delay_shoot;
        gun_shot_speed = gun.speed_shot;

        shot_rect->x = gun.x;
        shot_rect->y = gun.y;
        shot_rect->w = gun.w;
        shot_rect->h = gun.h;

        this->point_x = gun.point_x;
        this->point_y = gun.point_y;
    }

    void Update() {
        anim_speed = (in_movement) ? 20 : 60;
        anim_frame = (in_movement) ? 3 : 1;
        
        index_frame++;

        if (index_frame > anim_speed) {
            frame++;
            sheet_rect->x += 16;
            index_frame = 0;
        }

        if (frame > anim_frame) {
            frame = anim_frame-1;
            sheet_rect->x = frame*16;
            index_frame = 0;
        }
        
        // Movements
        this->rect->x += dx;
        this->rect->y += dy;

        this->gun_rect->x = this->rect->x + shot_rect->x;
        this->gun_rect->y = this->rect->y + shot_rect->y;

        in_movement = (dx != 0 || dy != 0) ? true : false;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, sheet_rect, rect);
        for (int i{0}; i < clothes.size(); ++i) {
            SDL_RenderCopy(renderer, clothes_textures.at(clothes.at(i)), sheet_rect, rect);
        }

        SDL_RenderCopy(renderer, gun_texture, gun_sheet_rect, gun_rect);
    }

    SDL_Rect* rect;
    SDL_Rect* sheet_rect;

    SDL_Rect* gun_rect;
    SDL_Rect* gun_sheet_rect;
    SDL_Rect* shot_rect;

    float dx, dy, speed, d_gun[2];
    bool shooting, can_shoot;

    Uint8 gun_time_shoot, gun_shot_speed, point_x, point_y;
    bool in_movement;
private:

    int anim_speed, anim_frame;

    Uint8 frame, index_frame;

    SDL_Texture* texture;
    SDL_Texture* gun_texture;
    std::vector<unsigned short int> clothes;
};

class Shot {
public:
    Shot(int x, int y, int w, int h) {
        speed = 15;

        rect = new SDL_Rect {x, y, w, h};

        alive = true;
    }

    ~Shot() {
        delete rect;
    }

    void Update() {
        rect->x += speed;

        if (rect->x > 740)
            alive = false;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 245, 236, 66, 255);
        SDL_RenderFillRect(renderer, rect);
    }

    float speed;
    bool alive;
    SDL_Rect* rect;
private:

};

class Item {
public:
    Item() {
        alive = true;
        frame = (rand()%3);

        rect = new SDL_Rect {(rand()%692), -48, 48, 48};
        sheet_rect = new SDL_Rect {frame*16, 0, 16, 16};
    }

    ~Item() {
        delete rect;
        delete sheet_rect;
    }

    void Update() {
        rect->y += 1;

        if (rect->y > 580) alive = false;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, item_texture, sheet_rect, rect);
    }

    bool alive;

    SDL_Rect* rect;
    Uint8 frame;

private:

    SDL_Rect* sheet_rect;
};

class GUI {
public:
    GUI() {
        
    }

    ~GUI() {
        TTF_CloseFont(font);
    }

    void Update(SDL_Renderer* renderer) {
        hp_text.SetText("Hp: "+std::to_string(hp_val));
        coins_text.SetText("Coins: "+std::to_string(coins_val));

        hp_text.Update(renderer);
        coins_text.Update(renderer);
    }

    void Render(SDL_Renderer* renderer) {
        hp_text.Render(renderer);
        coins_text.Render(renderer);
    }

    uint_fast32_t coins_val = 0;
    uint_fast32_t hp_val = 100;

private:
    Text hp_text {"Hp: ", 10, 10};
    Text coins_text {"Coin: ", 10, 30};
};

class Scene {
public:

    virtual ~Scene(){};

    bool pause;

    SDL_Renderer* renderer;

    virtual void Init(SDL_Renderer*) {};
    virtual void InputDown(SDL_Keycode) {};
    virtual void InputUp(SDL_Keycode) {};
    virtual void Update() {};
    virtual void Render() {};
protected:
};

class GamePlay : public Scene {
public:
    GamePlay() {}
    ~GamePlay() {
        delete player;

        for (int i{0}; i < zombies.size(); ++i) {
            printf("Deleting remaining zombie... number(%d/%d)\r", i+1, zombies.size());
            delete zombies.at(i);
        }
        putchar('\n');

        for (int i{0}; i < shots.size(); ++i) {
            printf("Deleting remaining Shots... number(%d/%d)\r", i+1, shots.size());
            delete shots.at(i);
        }
        putchar('\n');

        for (int i{0}; i < items.size(); ++i) {
            printf("Deleting remaining items... number(%d/%d)\r", i+1, items.size());
            delete items.at(i);
        }
        putchar('\n');
    }

    void Init(SDL_Renderer* sdl_renderer) {
        this->renderer = sdl_renderer;

        this->gui.Update(this->renderer);
        this->timer_spawner = 0;
        this->timer_shoot = 0;
        this->timer_items = 0;

        this->player = new Player();
        this->player->LoadTexture(renderer);
        this->player->ChangeGun(slot[0]);
    }

    void InputDown(SDL_Keycode key_code) {
        switch (key_code) {
            case SDLK_a:
                player->dx = -player->speed;
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
    }

    void InputUp(SDL_Keycode action_up) {
        if (action_up == SDLK_a || action_up == SDLK_d)     player->dx = 0;
        if (action_up == SDLK_s || action_up == SDLK_w)     player->dy = 0;
        if (action_up == SDLK_SPACE)                        player->shooting = false;
                    
    }

    void Update() {
        // Timers
        this->TimerControl();
        
        // Entities
        player->Update();
        player_y = player->rect->y;
        this->CollisionControl();
    }

    void Render() {
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
    }

private:
    /* Integers */
    Uint8 timer_spawner, timer_items, timer_shoot, slot_index;

    /* Groups */
    std::vector<Zombie*> zombies;
    std::vector<Shot*> shots;
    std::vector<Item*> items;

    /* Engine */
    GUI gui;
    Gun slot[2] = {glock, m4};

    Player* player;

    /* Others... */
    void PlayerShoot() {
        Shot* ns = new Shot(player->rect->x+player->point_x, player->rect->y+player->point_y, player->shot_rect->w, player->shot_rect->h);
        ns->speed = player->gun_shot_speed;
        shots.push_back(ns);
    }

    /* Controls*/
    void CollisionControl() {
        /* Zombies */
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
                    zombies[z]->Hit(slot[slot_index].damage, shots[i]->rect->y);
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
    }

    void TimerControl() {
        timer_spawner++;
        timer_items++;

        if (player->shooting) timer_shoot++;
        
        if (timer_shoot > player->gun_time_shoot) {
            PlayerShoot();
            timer_shoot = 0;
        }

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
    }
};

class PlayerEditor : public Scene {
    PlayerEditor() {}
    ~PlayerEditor() {
        SDL_DestroyTexture(head);
        SDL_DestroyTexture(body);
        SDL_DestroyTexture(foot);

        delete rect;
        delete sheet;
    }

    void Init(SDL_Renderer* sdl_renderer) {
        this->renderer = sdl_renderer;

        SDL_Surface* s;

        player = SDL_CreateTextureFromSurface(sdl_renderer, s);
        SDL_FreeSurface(s);
    }

    void InputDown(SDL_Keycode key) {
        switch (key)
        {
        case SDLK_LEFT:
            
            break;
        case SDLK_RIGHT:
            /* code */
            break;
        
        default:
            break;
        }
    }

    void InputUp(SDL_Keycode key) {

    }

    void Update() {

    }

    void Render() {
        SDL_RenderCopy(renderer, player, sheet, rect);
    }
private:
    int clothes_index[3];
    int cloth_type;

    SDL_Rect *rect, *sheet;

    SDL_Texture *player;
    SDL_Texture *head, *body, *foot;
};

void InitGame(SDL_Renderer *r) {
    TTF_Init();
    font = TTF_OpenFont("data/Minecraft.ttf", 24);

    // Create Textures
    clothes_textures.push_back(SDL_CreateTextureFromSurface(r, IMG_Load("data/players/clothes/dolman-body.png")));
    clothes_textures.push_back(SDL_CreateTextureFromSurface(r, IMG_Load("data/players/clothes/dolman-head.png")));
    clothes_textures.push_back(SDL_CreateTextureFromSurface(r, IMG_Load("data/players/clothes/juliet-glasses.png")));

    zombie_funk_texture = SDL_CreateTextureFromSurface(r, IMG_Load("data/zombies/zombie-funk-sheet.png"));
    zombie_brick_texture = SDL_CreateTextureFromSurface(r, IMG_Load("data/zombies/zombie-bricklayer-sheet.png"));
    zombie_normal_texture = SDL_CreateTextureFromSurface(r, IMG_Load("data/zombies/zombie-normal-sheet.png"));
    item_texture = SDL_CreateTextureFromSurface(r, IMG_Load("data/items-spritesheet.png"));

    // Zombie Textures Load
    zombie_textures.push_back(zombie_normal_texture);
    zombie_textures.push_back(zombie_brick_texture);
    zombie_textures.push_back(zombie_funk_texture);
    zombie_textures.push_back(item_texture);
    
    zom_upt.push_back(NormalZombie); // Normal Zombie
    zom_upt.push_back(NormalZombie); // Bricklayer
    zom_upt.push_back(FunkZombie);   // Funkero

    // Weapons
    m4.SetBulletSize(8, 4);
    m4.SetBulletPosition(80, 49);
    m4.SetDamage(30);

    glock.SetBulletSize(5, 3);
    glock.SetBulletPosition(60, 49);
    glock.SetDamage(15);
}

void EndGame() {
    // Free
    for (int i{0}; i < zombie_textures.size(); ++i) {
        SDL_DestroyTexture(zombie_textures.at(i));
    }

    for (int i{0}; i < clothes_textures.size(); ++i) {
        SDL_DestroyTexture(clothes_textures.at(i));
    }

    // Free Surface & Textures
    SDL_DestroyTexture(zombie_funk_texture);
    SDL_DestroyTexture(zombie_normal_texture);
    SDL_DestroyTexture(zombie_brick_texture);
    SDL_DestroyTexture(item_texture);

    TTF_CloseFont(font);
    TTF_Quit();
}