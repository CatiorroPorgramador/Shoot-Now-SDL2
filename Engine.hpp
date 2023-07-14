#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <string>

SDL_Surface* zombie_normal_surface = IMG_Load("data/zombies/zombie-normal-sheet.png");
SDL_Surface* zombie_brick_surface = IMG_Load("data/zombies/zombie-bricklayer-sheet.png");
SDL_Surface* item_surface = IMG_Load("data/items-spritesheet.png");

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

class Zombie {
public:
    Zombie() {
        rect = SDL_Rect {740, (rand()%490), 90, 90};
        sheet_rect = SDL_Rect {0, 0, 16, 16};

        frame = 0;
        index_frame = 0;

        alive = true;
    }

    ~Zombie() {
        SDL_DestroyTexture(texture);
    }

    void LoadTexture(SDL_Renderer* renderer) {
        texture = SDL_CreateTextureFromSurface(renderer, zombie_normal_surface);
    }

    void Update() {
        this->rect.x -= 1;

        index_frame++;
        
        if (index_frame > 10) {
            frame++;
            sheet_rect.x += 16;
            index_frame = 0;
        }

        if (frame > 8) {
            frame = 0;
            sheet_rect.x = 0;
            index_frame = 0;
        }

        if (rect.x < -rect.w)
            alive = false;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, &sheet_rect, &rect);
    }

    SDL_Rect rect;
    SDL_Rect sheet_rect;

    bool alive;

protected:
    Uint8 frame, index_frame;
    SDL_Texture* texture;
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
    }

    ~Item() {
        SDL_DestroyTexture(texture);

        delete rect;
        delete sheet_rect;
    }

    void Load(SDL_Renderer* renderer) {
        texture = SDL_CreateTextureFromSurface(renderer, item_surface);
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

void EndGame() {
    SDL_FreeSurface(zombie_normal_surface);
    SDL_FreeSurface(zombie_brick_surface);
    SDL_FreeSurface(item_surface);
}