#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <windows.h>

struct Gun {
public:
    Gun(const char* _name, Uint8 _max_bullets, Uint8 _delay_shoot, float _speed_shot, int _frame) {
        this->name = (char*) _name;
        this->delay_shoot = _delay_shoot;
        this->speed_shot = _speed_shot;
        this->frame = _frame;
    }

    char* name;
    Uint8 max_bullets, delay_shoot, delay_index;
    int frame;
    float speed_shot;
};

class Zombie {
public:
    Zombie() {
        rect = new SDL_Rect();
        rect->x = 740;
        rect->y = (rand()%490);
        rect->w = 90;
        rect->h = 90;

        sheet_rect = new SDL_Rect {0, 0, 16, 16};

        frame = 0;
        index_frame = 0;

        alive = true;
    }

    ~Zombie() {
        SDL_DestroyTexture(texture);
        delete rect;
        delete sheet_rect;
    }

    void LoadTexture(SDL_Renderer* renderer) {
        SDL_Surface* surface = IMG_Load("data/enemy.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    void Update() {
        this->rect->x -= 1;

        index_frame++;
        if (frame > 8) {
            frame = 0;
            sheet_rect->x = 0;
            index_frame = 0;
        }
        else if (index_frame > 10) {
            frame++;
            sheet_rect->x += 16;
            index_frame = 0;
        }

        if (rect->x < -rect->w)
            alive = false;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, sheet_rect, rect);
    }

    SDL_Rect* rect;
    SDL_Rect* sheet_rect;

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

        speed = 3;

        shooting = false;
        can_shoot = true;

        printf("Player Has Created...\n");
    }

    ~Player() {
        SDL_DestroyTexture(texture);
        SDL_DestroyTexture(gun_texture);
        delete rect;
        delete sheet_rect;

        delete gun_rect;
        delete gun_sheet_rect;

        printf("Player Has Deleted...\n");
    }

    void LoadTexture(SDL_Renderer* renderer) {
        // Player
        SDL_Surface* surface = IMG_Load("data/player.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        
        // Gun
        surface = IMG_Load("data/guns-spritesheet.png");
        gun_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    void ChangeGun(Gun gun) {
        gun_sheet_rect->x = 32*gun.frame;
        gun_time_shoot = gun.delay_shoot;
        gun_shot_speed = gun.speed_shot;
    }

    void Update(SDL_Keycode key_dn, SDL_Keycode key_up) {
        // Movements
        this->rect->x += dx;
        this->rect->y += dy;

        this->gun_rect->x = this->rect->x + 10;
        this->gun_rect->y = this->rect->y - 15;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, sheet_rect, rect);
        SDL_RenderCopy(renderer, gun_texture, gun_sheet_rect, gun_rect);
    }

    SDL_Rect* rect;
    SDL_Rect* sheet_rect;

    SDL_Rect* gun_rect;
    SDL_Rect* gun_sheet_rect;

    float dx, dy, speed;
    bool shooting, can_shoot;

    Uint8 gun_time_shoot, gun_shot_speed;

private:
    SDL_Texture* texture;
    SDL_Texture* gun_texture;
};

class Shot {
public:
    Shot(int x, int y) {
        speed = 15;

        rect = new SDL_Rect {x, y, 8, 4};

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