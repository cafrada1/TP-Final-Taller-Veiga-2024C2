#ifndef DRAW_CRATE_H
#define DRAW_CRATE_H

#include <SDL2pp/SDL2pp.hh>
#include <map>
#include <memory>
#include "../../../common/events/items/crate.h"
#include "../sound.h"

class DrawCrate {
    private:

    SDL2pp::Renderer &renderer;

    std::shared_ptr<SDL2pp::Texture> texture;

    std::shared_ptr<SDL2pp::Chunk> breakingSound;

    std::shared_ptr<SDL2pp::Chunk> hitSound;

    Sound sound;

    public:
    DrawCrate(SDL2pp::Renderer &renderer);

    SDL2pp::Rect crateClip[4]; 

    void render(const CrateDTO& crate);

};
#endif