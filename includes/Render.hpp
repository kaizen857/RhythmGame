#ifndef _RENDER_HPP_
#define _RENDER_HPP_
#include <SDL2/SDL.h>
#include <memory>
class Render
{
private:
    std::shared_ptr<SDL_Renderer> mRenderer;

public:
    Render(std::shared_ptr<SDL_Window> window)
    {
        mRenderer = std::make_shared<SDL_Renderer>(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer);
    }
};

#endif