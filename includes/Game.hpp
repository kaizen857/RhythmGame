#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
// #include <chrono>
#include <cstring>
#include <queue>
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
    SDL_Event Event;
    // 窗口宽度，高度
    const std::int16_t Widge = 1600;
    const std::int16_t Height = 900;
    // SDL_IMG相关设定
    std::int32_t ImgFlags = IMG_INIT_JPG;
    const std::string ImgFile = "./res/bg.jpg";
    SDL_Texture *BackGround;
    std::vector<std::vector<Note>> map;
    // 字体相关设定
    TTF_Font *Font;
    const std::string FontFile = "./res/Font_GBK.ttf";
    SDL_Color FontColor = {0, 0, 0, 255};

    // 音乐相关设定
    Mix_Music *Music;
    const std::string MusicFile = "./res/audio.ogg";
    // 分数计数器
    Counter score;
    // 音符下落速度(400ms 走完Height  单位pix/ms)
    const Uint16 Speed = Height / 400;
    // 判定线位置
    SDL_Rect JudgeLine{Widge / 2 - 200, Height - 40, 400, 15};

    // 准确度判定(单位：ms)
    const Uint8 PerfectTiming = 25;
    const Uint8 GoodTiming = 75;
    const Uint8 BadTiming = 125;
    const Uint8 MissTiming = 175;

    // 辅助函数 绘制游戏界面
    void DrawInterface(void)
    {
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);
        SDL_RenderCopy(Renderer, BackGround, NULL, NULL);
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(Renderer, Widge / 2 - 200, 0, Widge / 2 - 200, Height);
        SDL_RenderDrawLine(Renderer, Widge / 2 - 100, 0, Widge / 2 - 100, Height);
        SDL_RenderDrawLine(Renderer, Widge / 2, 0, Widge / 2, Height);
        SDL_RenderDrawLine(Renderer, Widge / 2 + 100, 0, Widge / 2 + 100, Height);
        SDL_RenderDrawLine(Renderer, Widge / 2 + 200, 0, Widge / 2 + 200, Height);
        SDL_RenderFillRect(Renderer, &JudgeLine);
    }

    // 辅助函数 中途退出
    void Quit(void)
    {
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyWindow(Window);
        TTF_CloseFont(Font);
        Mix_FreeMusic(Music);
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

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
        std::ofstream out("score.txt");
        out << score.GetPerfect() << "\n"
            << score.GetGood() << "\n"
            << score.GetBad() << "\n"
            << score.GetMiss() << "\n";
        out.close();
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
        std::ifstream in(path);
        if (!in.is_open())
        {
            throw(std::runtime_error("Can't open file"));
            std::exit(EXIT_FAILURE);
        }
        // int32_t start, end, key, type;
        std::int32_t start, end, key, type, LastStart = -1, cnt = 0;
        in >> start >> end >> key >> type;
        map.push_back(std::vector<Note>());
        NoteType temp;
        if (type == 0)
        {
            temp = NoteType::Single;
        }
        else
        {
            temp = NoteType::Strip;
        }
        map[cnt].push_back({start, end, key, temp});
        LastStart = start;
        while (in >> start >> end >> key >> type)
        {
            if (type == 0)
            {
                temp = NoteType::Single;
            }
            else
            {
                temp = NoteType::Strip;
            }
            if (start == LastStart)
            {
                map[cnt].push_back({start, end, key, temp});
                LastStart = start;
            }
            else
            {
                map.push_back(std::vector<Note>());
                cnt++;
                map[cnt].push_back({start, end, key, temp});
                LastStart = start;
            }
        }
        in.close();

        // 加载游戏界面纹理
    }
    void ShowMenu(void)
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
        // 设置字体大小
        TTF_SetFontSize(Font, 20);
        // 显示左上角歌曲信息
        auto SongName = TTF_RenderUTF8_Blended(Font, "Song Name:Rainbow Rush Story", FontColor);
        auto SongDifficulty = TTF_RenderUTF8_Blended(Font, "Map Difficulty:Standard", FontColor);
        auto SongArtist = TTF_RenderUTF8_Blended(Font, "Song Artist:いるかアイス feat. ちょこ", FontColor);

        auto SongNameTexture = SDL_CreateTextureFromSurface(Renderer, SongName);
        auto SongDifficultyTexture = SDL_CreateTextureFromSurface(Renderer, SongDifficulty);
        auto SongArtistTexture = SDL_CreateTextureFromSurface(Renderer, SongArtist);

        // 确定位置
        SDL_Rect SongNamePos, SongDifficultyPos, SongArtistPos;
        SongNamePos.x = SongDifficultyPos.x = SongArtistPos.x = 0;
        SongNamePos.y = 0;
        SDL_QueryTexture(SongNameTexture, NULL, NULL, &SongNamePos.w, &SongNamePos.h);
        SDL_QueryTexture(SongDifficultyTexture, NULL, NULL, &SongDifficultyPos.w, &SongDifficultyPos.h);
        SDL_QueryTexture(SongArtistTexture, NULL, NULL, &SongArtistPos.w, &SongArtistPos.h);
        SongDifficultyPos.y = SongNamePos.y + SongNamePos.h + 10;
        SongArtistPos.y = SongDifficultyPos.y + SongDifficultyPos.h + 10;
        // 将材质复制进渲染器
        SDL_RenderCopy(Renderer, SongNameTexture, NULL, &SongNamePos);
        SDL_RenderCopy(Renderer, SongDifficultyTexture, NULL, &SongDifficultyPos);
        SDL_RenderCopy(Renderer, SongArtistTexture, NULL, &SongArtistPos);
        SDL_RenderPresent(Renderer);
    }
    void Run() // 下落速度400ms
    {
        std::uint8_t alpha = 255;
        // 调整背景图不透明度
        while (alpha > 76)
        {
            SDL_SetTextureAlphaMod(BackGround, alpha);
            SDL_RenderCopy(Renderer, BackGround, NULL, NULL);
            SDL_RenderPresent(Renderer);
            alpha -= 5;
            SDL_Delay(10);
            SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
            SDL_RenderClear(Renderer);
        }
        // 渲染游戏界面
        DrawInterface();
        // Mix_PlayMusic(Music, 1);
        SDL_RenderPresent(Renderer);
        auto StartTime = SDL_GetTicks();
        std::queue<ShowNote> RenderQueue;
        std::int32_t ShowIndex = 0;
        auto LastTime = SDL_GetTicks();
        std::ofstream out("info.txt");
        Uint32 JudgePoint = 0;
        bool IsQuit = false;
        while (!IsQuit) // 游戏内循环
        {
            // 获取当前帧时间
            auto NowTime = SDL_GetTicks();
            out << "nowtime:" << NowTime << "\n";
            // 添加note进渲染队列
            if (ShowIndex < map.size() && map[ShowIndex][0].GetStartTime() - 400 <= NowTime - StartTime)
            {
                for (auto &i : map[ShowIndex])
                {
                    auto Key = i.GetKey();
                    auto Type = i.GetType();
                    if (Type == NoteType::Single)
                    {
                        int x = Widge / 2 - 200 + (Key - 1) * 100;
                        int y = 0;
                        RenderQueue.push({x, y, 100, 20, i});
                    }
                }
                ShowIndex++;
            }
            // 渲染note以及游戏界面（同时判定note）
            auto Size = RenderQueue.size();
            bool IsJudge[4];
            memset(IsJudge, 0, sizeof(IsJudge));
            DrawInterface();
            for (auto i = 0; i < Size; i++)
            {
                auto Now = RenderQueue.front();
                RenderQueue.pop();
                // 如果当前音符已经到达画面底部但仍未被判定
                if (Now.GetYPos() >= Height && Now.GetIsJudge() == false)
                {
                    score.PlusMiss(); // miss+1
                    Size--;
                    continue; // 并丢弃音符
                }
                SDL_PollEvent(&Event);
                if (Event.type == SDL_QUIT) // 先判定键盘事件
                {
                    IsQuit = true;
                    break;
                }
                else if (Event.type == SDL_KEYDOWN) // 对输入事件进行判定
                {
                    auto ReqKey = Now.GetKey();
                    if (ReqKey == 0)
                    {
                        ReqKey = SDLK_a;
                    }
                    else if (ReqKey == 1)
                    {
                        ReqKey = SDLK_s;
                    }
                    else if (ReqKey == 2)
                    {
                        ReqKey = SDLK_k;
                    }
                    else
                    {
                        ReqKey = SDLK_l;
                    }
                    if (IsJudge[Now.GetKey()] == false) // 如果当前轨道无音符被判定则进入判定（防止提早判定）
                    {
                        auto dif = (NowTime - StartTime) - Now.GetStartTime();
                        if (dif < 0) // 比判定时间更早点到（不判定miss）
                        {
                            dif = std::abs(dif);
                            if (Event.key.keysym.sym == ReqKey)
                            {
                                if (dif <= PerfectTiming)
                                {
                                    score.PlusPerfect();
                                    Now.SetIsJudge(true);
                                    IsJudge[Now.GetKey()] = true;
                                    Size--;
                                }
                                else if (dif <= GoodTiming)
                                {
                                    score.PlusGood();
                                    Now.SetIsJudge(true);
                                    IsJudge[Now.GetKey()] = true;
                                    Size--;
                                }
                                else if (dif <= BadTiming)
                                {
                                    score.PlusBad();
                                    Now.SetIsJudge(true);
                                    IsJudge[Now.GetKey()] = true;
                                    Size--;
                                }
                            }
                        }
                        else // 比判定时间更晚点到（判定Miss）
                        {
                            if (Event.key.keysym.sym == ReqKey)
                            {
                                if (dif <= PerfectTiming)
                                {
                                    score.PlusPerfect();
                                }
                                else if (dif <= GoodTiming)
                                {
                                    score.PlusGood();
                                }
                                else if (dif <= BadTiming)
                                {
                                    score.PlusBad();
                                }
                                else
                                {
                                    score.PlusMiss();
                                }
                                Now.SetIsJudge(true);
                                IsJudge[Now.GetKey()] = true;
                                Size--;
                            }
                        }
                    }
                }
                // 更新画面（如果note已做判定则不进行渲染）
                if (!Now.GetIsJudge())
                {
                    auto S = Speed * (NowTime - LastTime);
                    Now.PlusRectYPos(S);
                    Now.Draw(Renderer);
                    RenderQueue.push(Now); // 将音符重新放入渲染队列
                }
            }
            SDL_RenderPresent(Renderer);
            LastTime = NowTime;
            if (NowTime > 10000)
            {
                IsQuit = true;
            }
        }
        out.close();
        SDL_Delay(10000);
    }
};

#endif