#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
        alive = true;

        rect = new SDL_Rect();
        rect->x = 740;
        rect->y = (rand()%520);
        rect->w = 60;
        rect->h = 60;
    }

    ~Zombie() {
        SDL_DestroyTexture(texture);
        delete rect;
    }

    void LoadTexture(SDL_Renderer* renderer) {
        SDL_Surface* surface = IMG_Load("data/enemy.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    void Update() {
        this->rect->x -= 1;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, NULL, rect);
    }

    SDL_Rect* rect;

    bool alive;

protected:
    SDL_Texture* texture;
};

class Player {
public:
    Player() {
        rect = new SDL_Rect();
        rect->x = 0;
        rect->x = 0;
        rect->w = 16*4;
        rect->h = 16*4;

        gun_rect = new SDL_Rect();
        gun_rect->x = 0;
        gun_rect->x = 0;
        gun_rect->w = 105;
        gun_rect->h = 105;

        gun_sheet_rect = new SDL_Rect();
        gun_sheet_rect->x = 0;
        gun_sheet_rect->y = 0;
        gun_sheet_rect->w = 32;
        gun_sheet_rect->h = 32;

        speed = 3;

        shooting = false;
        can_shoot = true;

        printf("Player Has Created...\n");
    }

    ~Player() {
        SDL_DestroyTexture(texture);
        SDL_DestroyTexture(gun_texture);
        delete rect;
        delete gun_rect;

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
        SDL_RenderCopy(renderer, texture, NULL, rect);
        SDL_RenderCopy(renderer, gun_texture, gun_sheet_rect, gun_rect);
    }

    SDL_Rect* rect;
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

        rect = new SDL_Rect();

        rect->x = x;
        rect->y = y;
        rect->w = 8;
        rect->h = 4;

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