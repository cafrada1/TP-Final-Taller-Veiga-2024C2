#include "draw_chestplate.h"
#include <iostream>
#include <memory>
#define DATA_PATH "../client/sprites/"

DrawChestplate::DrawChestplate(SDL2pp::Renderer &renderer, bool equipped)
    : renderer(renderer), equipped(equipped) {
  this->textures[0] = std::make_shared<SDL2pp::Texture>(
      renderer, SDL2pp::Surface(DATA_PATH "equipment/chestplate.png"));
  this->textures[1] = std::make_shared<SDL2pp::Texture>(
      renderer,
      SDL2pp::Surface(DATA_PATH "equipment/chestplate-equipped-1.png"));
}

void DrawChestplate::render(int x, int y, bool direction) {
  int chestplateId = equipped ? 1 : 0;
  std::shared_ptr<SDL2pp::Texture> texture = this->textures[chestplateId];
  SDL2pp::Rect dest(x, y + 7, texture->GetWidth(), texture->GetHeight());
  renderer.Copy(*texture, SDL2pp::NullOpt, dest, 0, SDL2pp::NullOpt,
                direction ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
}

void DrawChestplate::update(const Chestplate &chestplate) {
  equipped = chestplate.is_equipped();
}

bool DrawChestplate::isEquipped() { return equipped; }