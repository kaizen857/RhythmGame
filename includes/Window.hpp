#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__
#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <memory>
#include <string>
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
    Window() = default;
    ~Window() = default;

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

    void CreateWindow(Uint16 width, Uint16 height, const char *title)
    {
        mWindow = std::make_shared<SDL_Window>(SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN), SDL_DestroyWindow);
    }

    void CreateWindow(Uint16 width, Uint16 height,const std::string &title)
    {
        mWindow = std::make_shared<SDL_Window>(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN), SDL_DestroyWindow);
    }
};

#endif