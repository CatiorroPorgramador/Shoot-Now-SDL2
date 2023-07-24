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

/* Zombies Updates: Differents Updates for differents zombies, to improve game fun */
std::vector<std::function<void(SDL_Rect*, SDL_Rect*, Uint8*, Uint8*)>> zom_upt;
std::vector<SDL_Texture*> zombie_textures;

SDL_Texture* zombie_funk_texture;
SDL_Texture* zombie_normal_texture;
SDL_Texture* zombie_brick_texture;
SDL_Texture* item_texture;

TTF_Font* font;

struct Text {
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

    char* name;
    Uint8 max_bullets, delay_shoot, delay_index, x, y, w, h, point_x, point_y; // Points are the shoots points start
    int frame;
    float speed_shot;
};

void NormalZombie(SDL_Rect* r, SDL_Rect* sr, Uint8 *f, Uint8 *i) {
    r->x -= 1;

    *i += 1;
    
    if (*i > 10) {
        *f += 1;
        sr->x += 16;
        *i = 0;
    }

    if (*f > 8) {
        *f = 0;
        sr->x = 0;
        *i = 0;
    }
}

void FunkZombie(SDL_Rect* r, SDL_Rect* sr, Uint8 *f, Uint8 *i) {
    *i += 1;

    if (r->x > 450) {
        r->x -= 1;

        if (*i > 15) {
            *f += 1;
            sr->x += 16;
            *i = 0;
        }

        if (*f > 8) {
            *f = 0;
            sr->x = 0;
            *i = 0;
        }
    } else {
        if (*f < 9) { 
            *f = 9;
            sr->x = 16*9;
            *i = 0;
        }

        if (*i > 30 && !(*f >=14)) {
            *f += 1;
            sr->x += 16;
            *i = 0;
        }
        else if (*i > 60 && (*f >= 14))
        {
            *f -= 1;
            sr->x -= 16;
            *i = 0;
        }

        if (*f > 14) {
            *f = 9;
            sr->x = 16 * (*f);
            *i = 0;
        }
    }
}

class Zombie {
public:
    Zombie() {
        rect = new SDL_Rect {740, (rand()%490), 90, 90};
        sheet_rect = new SDL_Rect {0, 0, 16, 16};

        frame = new Uint8(0);
        index_frame = new Uint8(8);

        alive = true;

        type = rand()%3;
        upt = zom_upt[type];
        texture = zombie_textures[type];
    }

    ~Zombie() {
        delete rect;
        delete sheet_rect;
        
        free(frame);
        free(index_frame);
    }

    void Update() {
        upt(rect, sheet_rect, frame, index_frame);

        if (rect->x < -rect->w) alive = false;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, sheet_rect, rect);
    }

    bool alive;

    SDL_Rect *rect;
    SDL_Rect *sheet_rect;

protected:
    Uint8 *frame, *index_frame, type;
    SDL_Texture* texture;
    std::function<void(SDL_Rect*, SDL_Rect*, Uint8*, Uint8*)> upt;

    float speed;
};

class Player {
public:
    Player() {
        rect = new SDL_Rect {0, 0, 16*4, 16*4};

        sheet_rect = new SDL_Rect {0, 0, 16, 16};

        gun_rect = new SDL_Rect {0, 0, 105, 105};

        gun_sheet_rect = new SDL_Rect {0, 0, 32, 32};

        shot_rect = new SDL_Rect {0, 0, 0, 0};

        speed = 3;

        shooting = false;
        can_shoot = true;

        dx = 0;
        dy = 0;

        d_gun[0] = 0;
        d_gun[1] = 0;

        frame = 2;

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

    void Update(SDL_Keycode key_dn, SDL_Keycode key_up) {
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

        texture = item_texture;
    }

    ~Item() {
        SDL_DestroyTexture(texture);

        delete rect;
        delete sheet_rect;
    }

    void Update() {
        rect->y += 1;

        if (rect->y > 580)
            alive = false;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, sheet_rect, rect);
    }

    bool alive;

    SDL_Rect* rect;
    Uint8 frame;

private:
    SDL_Texture* texture;

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

void InitGame(SDL_Renderer* r) {
    TTF_Init();
    font = TTF_OpenFont("data/Minecraft.ttf", 24);

    // Create Textures
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
}

void EndGame() {
    // Free
    for (int i{0}; i < zombie_textures.size(); ++i) {
        SDL_DestroyTexture(zombie_textures[i]);
    }

    // Free Surface & Textures
    SDL_DestroyTexture(zombie_funk_texture);
    SDL_DestroyTexture(zombie_normal_texture);
    SDL_DestroyTexture(zombie_brick_texture);
    SDL_DestroyTexture(item_texture);

    TTF_CloseFont(font);
}