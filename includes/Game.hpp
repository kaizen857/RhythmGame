#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "Note.hpp"

class Game
{
private:
    SDL_Window *Window;
    SDL_Renderer *Renderer;
    SDL_Event m_event;
    // 窗口宽度，高度
    const std::int16_t Widge = 1600;
    const std::int16_t Height = 900;
    // 界面，渲染器
    SDL_Surface *Surface;
    SDL_Texture *Texture;
    // SDL_IMG相关设定
    std::int32_t ImgFlags = IMG_INIT_JPG;
    const std::string ImgFile = "./res/bg.jpg";
    SDL_Texture *BackGround;
    std::vector<Note> map;
    // 字体相关设定
    TTF_Font *Font;
    const std::string FontFile = "./res/Font_GBK.ttf";
    SDL_Color FontColor = {0, 0, 0, 255};

    // 音乐相关设定
    Mix_Music *Music;
    const std::string MusicFile = "./res/audio.ogg";

    // 游戏分数计数
    Counter score;

    // 计时器
    

public:
    Game()
    {
        // SDL初始化
        auto res = SDL_Init(SDL_INIT_EVERYTHING);
        if (res != 0)
        {
            throw(std::runtime_error(SDL_GetError()));
            std::exit(EXIT_FAILURE);
        }
        // 初始化窗口
        Window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Widge, Height, SDL_WINDOW_SHOWN);
        if (!Window)
        {
            throw(std::runtime_error(SDL_GetError()));
            std::exit(EXIT_FAILURE);
        }
        // 设置纹理线性过滤
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"))
        {
            throw(std::runtime_error("Warning: Linear texture filtering not enabled!"));
            std::exit(EXIT_FAILURE);
        }
        // 初始化渲染器
        Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        // 初始化SDL_IMG库
        if (!(IMG_Init(ImgFlags) & ImgFlags))
        {
            throw(std::runtime_error(IMG_GetError()));
            std::exit(EXIT_FAILURE);
        }
        // 初始化TTF
        res = TTF_Init();
        if (res != 0)
        {
            throw(std::runtime_error(TTF_GetError()));
            std::exit(EXIT_FAILURE);
        }
        // 初始化Mixer
        Mix_Init(MIX_INIT_OGG);
        res = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_CHANNELS, 4096);
        if (res != 0)
        {
            throw(std::runtime_error(Mix_GetError()));
            std::exit(EXIT_FAILURE);
        }
    }

    ~Game()
    {
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyWindow(Window);
        TTF_CloseFont(Font);
        Mix_FreeMusic(Music);
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        // Mix_Quit();
    }

    void Init(std::string path)
    {
        // 加载背景图
        SDL_Surface *surface = IMG_Load(ImgFile.c_str());
        auto FinalSurface = SDL_ConvertSurface(surface, SDL_GetWindowSurface(Window)->format, 0);
        SDL_FreeSurface(surface);
        BackGround = SDL_CreateTextureFromSurface(Renderer, FinalSurface);
        SDL_FreeSurface(FinalSurface);
        SDL_SetTextureBlendMode(BackGround, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(BackGround, 255);
        SDL_RenderCopy(Renderer, BackGround, NULL, NULL);

        // 加载字体
        Font = TTF_OpenFont(FontFile.c_str(), 40);
        if (Font == NULL)
        {
            throw(std::runtime_error(TTF_GetError()));
            std::exit(EXIT_FAILURE);
        }
        // 加载音乐
        Music = Mix_LoadMUS(MusicFile.c_str());
        // 加载地图
        /*std::ifstream in(path);
        if (!in.is_open())
        {
            throw(std::runtime_error("Can't open file"));
            std::exit(EXIT_FAILURE);
        }
        // int32_t start, end, key, type;
        std::int32_t start, end, key, type;
        while (in >> start >> end >> key >> type)
        {
            NoteType temp;
            if (type == 0)
            {
                temp = NoteType::Single;
            }
            else
            {
                temp = NoteType::Strip;
            }
            map.push_back(Note(start, end, key, temp));
        }*/
    }
    void ShowStart(void)
    {
        // 加载开始界面（请按任意键开始）字体
        auto start = TTF_RenderUTF8_Blended(Font, "Press any key to start", FontColor);
        auto FontTexture = SDL_CreateTextureFromSurface(Renderer, start);
        SDL_Rect MessageRect;
        MessageRect.x = Widge / 2 - start->w / 2; // 居中
        MessageRect.y = Height / 2 - start->h / 2;
        SDL_QueryTexture(FontTexture, NULL, NULL, &MessageRect.w, &MessageRect.h);
        SDL_RenderCopy(Renderer, FontTexture, NULL, &MessageRect);
        SDL_FreeSurface(start);
        // 修改字体大小
        TTF_SetFontSize(Font, 20);
        // 界面左上角信息（乐曲名字，铺面难度）
        auto MusicName = TTF_RenderUTF8_Blended(Font, "Name:Rainbow Rush Story", FontColor);
        auto MusicDifficulty = TTF_RenderUTF8_Blended(Font, "Difficulty:Standard", FontColor);
        auto MusicArtist = TTF_RenderUTF8_Blended(Font, "Artist:いるかアイス feat. ちょこ", FontColor);

        auto MusicNameTexture = SDL_CreateTextureFromSurface(Renderer, MusicName);
        auto MusicDifficultyTexture = SDL_CreateTextureFromSurface(Renderer, MusicDifficulty);
        auto MusicArtistTexture = SDL_CreateTextureFromSurface(Renderer, MusicArtist);
        // 设定信息位置
        SDL_Rect MusicNameRect;
        SDL_Rect MusicDifficultyRect;
        SDL_Rect MusicArtistRect;
        MusicNameRect.x = 0;
        MusicNameRect.y = 0;
        SDL_QueryTexture(MusicNameTexture, NULL, NULL, &MusicNameRect.w, &MusicNameRect.h);
        MusicDifficultyRect.x = 0;
        MusicDifficultyRect.y = MusicNameRect.h + 10;
        SDL_QueryTexture(MusicDifficultyTexture, NULL, NULL, &MusicDifficultyRect.w, &MusicDifficultyRect.h);
        MusicArtistRect.x = 0;
        MusicArtistRect.y = MusicDifficultyRect.y + MusicDifficultyRect.h + 10;
        SDL_QueryTexture(MusicArtistTexture, NULL, NULL, &MusicArtistRect.w, &MusicArtistRect.h);
        // 渲染信息
        SDL_RenderCopy(Renderer, MusicNameTexture, NULL, &MusicNameRect);
        SDL_RenderCopy(Renderer, MusicDifficultyTexture, NULL, &MusicDifficultyRect);
        SDL_RenderCopy(Renderer, MusicArtistTexture, NULL, &MusicArtistRect);
        SDL_RenderPresent(Renderer);

        SDL_FreeSurface(MusicName);
        SDL_FreeSurface(MusicDifficulty);
        SDL_FreeSurface(MusicArtist);
        SDL_DestroyTexture(MusicNameTexture);
        SDL_DestroyTexture(MusicDifficultyTexture);
        SDL_DestroyTexture(MusicArtistTexture);
    }
    void Run() // 下落速度400ms
    {
        std::uint8_t alpha = 255;
        while (alpha > 76)
        {
            SDL_SetTextureAlphaMod(BackGround, alpha);
            SDL_RenderCopy(Renderer, BackGround, NULL, NULL);
            SDL_RenderPresent(Renderer);
            alpha -= 5;
            SDL_Delay(1);
            SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
            SDL_RenderClear(Renderer);
        }
        // Mix_PlayMusic(Music, 1);
        SDL_Delay(10000);
    }
};

#endif