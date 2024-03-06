#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <memory>
class Window
{
private:
    Uint16 Width, Height;
    std::shared_ptr<SDL_Window> mWindow;

public:
    Window(Uint16 width, Uint16 height, const char *title)
    {
        mWindow = std::make_shared<SDL_Window>(SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN), SDL_DestroyWindow);
    };
    ~Window();

    // Getters
    auto getWidth() const
    {
        return Width;
    }
    auto getHeight() const
    {
        return Height;
    }

    auto getWindow()
    {
        return mWindow;
    }

    // setters
    void setWidth(Uint16 width)
    {
        Width = width;
    }
    void setHeight(Uint16 height)
    {
        Height = height;
    }
};

#endif