#include <iostream>
#include "Player.h"


bool Player::Moved() const {
    if (coords.x == old_coords.x && coords.y == old_coords.y)
        return false;
    else
        return true;
}

void Player::ProcessInput(MovementDir dir) {
    int move_dist = move_speed * 1;
    switch (dir) {
        case MovementDir::UP:
            old_coords.y = coords.y;
            coords.y += move_dist;
            break;
        case MovementDir::DOWN:
            old_coords.y = coords.y;
            coords.y -= move_dist;
            break;
        case MovementDir::LEFT:
            old_coords.x = coords.x;
            coords.x -= move_dist;
            break;
        case MovementDir::RIGHT:
            old_coords.x = coords.x;
            coords.x += move_dist;
            break;
        default:
            break;
    }
}

void Player::Draw(Image &screen, ScreenState& screen_state) {
    if (Moved()) {
        for (int y = old_coords.y; y <= old_coords.y + tileSize; ++y) {
            for (int x = old_coords.x; x <= old_coords.x + tileSize; ++x) {
                screen.PutPixel(x, y, screen_state.background_state[y*h_WINDOW_WIDTH+x]);  // todo create function
            }
        }
        old_coords = coords;
    }
    for (int y = coords.y; y <= coords.y + tileSize; ++y) {
        for (int x = coords.x; x <= coords.x + tileSize; ++x) {

            Pixel oldPix = screen_state.background_state[y*h_WINDOW_WIDTH+x];
            Pixel newPix = this->player_image.GetPixel(x - coords.x, tileSize - y + coords.y);
            screen.PutPixel(x, y, blend(oldPix, newPix));
//            Pixel masha = andrew.GetPixel(x-coords.x, y-coords.y);
//            screen.PutPixel(x, y, masha);
        }
    }
}