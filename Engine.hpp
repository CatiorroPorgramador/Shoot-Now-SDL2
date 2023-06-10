#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <windows.h>

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

        shooting = false;
        can_shoot = true;

        printf("Player Has Created...\n");
    }

    ~Player() {
        SDL_DestroyTexture(texture);
        delete rect;
        delete gun_rect;

        printf("Player Has Deleted...\n");
    }

    void LoadTexture(SDL_Renderer* renderer) {
        SDL_Surface* surface = IMG_Load("data/player.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        surface = IMG_Load("data/guns-spritesheet.png");
        gun_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    void ChangeGun() {
        
    }

    void Update(SDL_Keycode key_dn, SDL_Keycode key_up) {
        this->gun_rect->x = this->rect->x + 10;
        this->gun_rect->y = this->rect->y - 15;

        // Movements
        this->rect->x += dx;
        this->rect->y += dy;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, texture, NULL, rect);
        SDL_RenderCopy(renderer, gun_texture, NULL, gun_rect);
    }

    SDL_Rect* rect;
    SDL_Rect* gun_rect;

    float dx, dy;
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