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
    std::int32_t ImgFlags = IMG_INIT_PNG;
    const std::string ImgFile = "./res/bg.png";
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
    // 判定线位置
    SDL_FRect JudgeLine{static_cast<float>(Widge) / 2.0f - 200.0f, static_cast<float>(Height) - 80.0f, 400.0f, 15.0f};
    // 显示判定的位置
    SDL_FRect DisplayJudge{static_cast<float>(Widge) / 2.0f, static_cast<float>(Height) / 2.0f - 100.0f, 200.0f, 50.0f};
    // 轨道宽度
    const Uint16 TrackWidth = 100;
    // note高度
    const Uint16 NoteHeight = 25;
    // 距离判定显示后的时间
    Uint32 DisplayTime = 0;
    // 判定显示的最大时间
    const Uint32 MaxDisplayTime = 1000;
    // 轨道数
    const Uint8 TrackNum = 4;
    // 音符下落时间(单位ms)
    const Uint32 NoteDownSpeed = 500;
    // 音符下落速度(单位pix/ms)
    const double Speed = static_cast<double>(JudgeLine.y) / static_cast<double>(NoteDownSpeed);
    // 准确度判定(单位：ms)
    const Uint8 PerfectTiming = 50;
    const Uint8 GoodTiming = 100;
    const Uint8 BadTiming = 150;
    const Uint8 MissTiming = 200;
    // 键位
    SDL_KeyCode KeyCodeForFour[4] = {SDLK_a, SDLK_s, SDLK_k, SDLK_l};

    // 游戏note材质
    const std::string NoteFile = "./res/mania/mania_note1.png";
    SDL_Texture *NoteTexture;
    const std::string StripFile = "./res/mania/B.png";
    SDL_Texture *StripTexture;
    // 游戏判定相关材质
    const std::string ShowPerfectFile = "./res/mania/mania_hitPerfect@2x.png";
    const std::string ShowGoodFile = "./res/mania/mania_hitGood@2x.png";
    const std::string ShowBadFile = "./res/mania/mania_hitBad@2x.png";
    const std::string ShowMissFile = "./res/mania/mania_hitMiss@2x.png";
    SDL_Texture *PerfectTexture;
    SDL_Texture *GoodTexture;
    SDL_Texture *BadTexture;
    SDL_Texture *MissTexture;
    // 显示游戏判定
    SDL_Texture *NowShowTexture = nullptr;
    // 显示游戏判定时间
    Uint32 ShowJudgeTime = 1000;
    Uint32 NowShowJudgeTime = 0;
    bool IsNowShowJudge = false;

    SDL_FRect ShowJudgePos{static_cast<float>(Widge) / 2.0f, static_cast<float>(Height) / 2.0f - 100};

    // 渲染队列
    std::queue<ShowNote> *RenderQueue;
    // 退出标志
    bool IsQuit = false;
    // 游戏开始时间
    Uint32 StartTime;
    // 上一帧时间
    Uint32 LastTime;
    // 当前帧时间
    Uint32 CurrentTime;
    // 添加note的索引
    Uint32 NoteIndex;
    // 距离开始游戏的时间
    Uint32 StartGameTime;
    // Perfect等显示时间
    Uint32 ShowTime;
    // Perfect等显示时长
    Uint32 CurrentShowTime;

    std::ofstream out;

    // 辅助函数 绘制游戏界面
    void DrawInterface(void)
    {
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);
        SDL_RenderCopy(Renderer, BackGround, NULL, NULL);
        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(Renderer, Widge / 2 - (TrackWidth * 2), 0, Widge / 2 - (TrackWidth * 2), Height);
        SDL_RenderDrawLine(Renderer, Widge / 2 - TrackWidth, 0, Widge / 2 - TrackWidth, Height);
        SDL_RenderDrawLine(Renderer, Widge / 2, 0, Widge / 2, Height);
        SDL_RenderDrawLine(Renderer, Widge / 2 + TrackWidth, 0, Widge / 2 + TrackWidth, Height);
        SDL_RenderDrawLine(Renderer, Widge / 2 + (TrackWidth * 2), 0, Widge / 2 + (TrackWidth * 2), Height);
        SDL_RenderFillRectF(Renderer, &JudgeLine);
    }
    // 判定
    void Judge(void)
    {
        if (SDL_PollEvent(&Event))
        {
            for (std::int32_t i = 0; i < TrackNum; i++)
            {
                auto &note = RenderQueue[i].front();
                SDL_Keycode ReqKey = KeyCodeForFour[i];

                if (Event.type == SDL_QUIT)
                {
                    Quit();
                }
                else if (Event.type == SDL_KEYDOWN)
                {
                    out << "time after start:" << StartGameTime << "  ";
                    out << "in:" << i << " key is :" << Event.key.keysym.sym << "req key is:" << ReqKey << "\n";
                    if (Event.key.keysym.sym == ReqKey)
                    {
                        auto diff = StartGameTime - note.GetStartTime();
                        if (diff < 0) // fast
                        {
                            diff = std::abs(diff);
                            if (diff < PerfectTiming)
                            {
                                score.PlusPerfect();
                                out << " res: perfect(early)"
                                    << "\n";
                                note.SetIsJudge(true);
                                if (IsNowShowJudge)
                                {
                                    NowShowJudgeTime = 0;
                                }
                                else
                                {
                                    IsNowShowJudge = true;
                                    NowShowJudgeTime = 0;
                                }
                                NowShowTexture = PerfectTexture;
                                break;
                            }
                            else if (diff < GoodTiming)
                            {
                                score.PlusGood();
                                out << " res: good(early)"
                                    << "\n";
                                note.SetIsJudge(true);
                                if (IsNowShowJudge)
                                {
                                    NowShowJudgeTime = 0;
                                }
                                else
                                {
                                    IsNowShowJudge = true;
                                    NowShowJudgeTime = 0;
                                }
                                NowShowTexture = GoodTexture;
                                break;
                            }
                            else if (diff < BadTiming)
                            {
                                score.PlusBad();
                                out << " res: bad(early)"
                                    << "\n";
                                note.SetIsJudge(true);
                                if (IsNowShowJudge)
                                {
                                    NowShowJudgeTime = 0;
                                }
                                else
                                {
                                    IsNowShowJudge = true;
                                    NowShowJudgeTime = 0;
                                }
                                NowShowTexture = BadTexture;
                                break;
                            }
                        }
                        else // late
                        {
                            diff = std::abs(diff);
                            if (diff < PerfectTiming)
                            {
                                out << " res: perfect(late)"
                                    << "\n";
                                score.PlusPerfect();
                                if (IsNowShowJudge)
                                {
                                    NowShowJudgeTime = 0;
                                }
                                else
                                {
                                    IsNowShowJudge = true;
                                    NowShowJudgeTime = 0;
                                }
                                NowShowTexture = PerfectTexture;
                            }
                            else if (diff < GoodTiming)
                            {
                                score.PlusGood();
                                out << " res: good(late)"
                                    << "\n";
                                if (IsNowShowJudge)
                                {
                                    NowShowJudgeTime = 0;
                                }
                                else
                                {
                                    IsNowShowJudge = true;
                                    NowShowJudgeTime = 0;
                                }
                                NowShowTexture = GoodTexture;
                            }
                            else if (diff < BadTiming)
                            {
                                score.PlusBad();
                                out << " res: bad(late)"
                                    << "\n";
                                if (IsNowShowJudge)
                                {
                                    NowShowJudgeTime = 0;
                                }
                                else
                                {
                                    IsNowShowJudge = true;
                                    NowShowJudgeTime = 0;
                                }
                                NowShowTexture = BadTexture;
                            }
                            else if (diff < MissTiming)
                            {
                                score.PlusMiss();
                                out << " res: miss(late)"
                                    << "\n";
                                if (IsNowShowJudge)
                                {
                                    NowShowJudgeTime = 0;
                                }
                                else
                                {
                                    IsNowShowJudge = true;
                                    NowShowJudgeTime = 0;
                                }
                                NowShowTexture = MissTexture;
                            }
                            note.SetIsJudge(true);
                            break;
                        }
                    }
                }
            }
        }
    }
    // 显示判定
    void ShowJudge(void)
    {
        if (IsNowShowJudge)
        {
            SDL_RenderCopyF(Renderer, NowShowTexture, nullptr, &ShowJudgePos);
            NowShowJudgeTime += (CurrentTime - LastTime);
            if (NowShowJudgeTime >= ShowJudgeTime)
            {
                IsNowShowJudge = false;
                NowShowTexture = nullptr;
                NowShowJudgeTime = 0;
            }
        }
    }
    // 渲染
    void Show(void)
    {
        for (auto i = 0; i < TrackNum; i++)
        {
            auto Size = RenderQueue[i].size();
            for (auto j = 0; j < Size; j++)
            {
                auto note = RenderQueue[i].front();
                RenderQueue[i].pop();
                if (note.GetType() == NoteType::Single) // 单键
                {
                    if (note.GetIsJudge() == true)
                    {
                        continue;
                    }
                    else
                    {
                        note.Draw(Renderer, NoteTexture);
                        auto Distence = Speed * (CurrentTime - LastTime);
                        note.PlusRectYPos(Distence);
                        if (note.GetYPos() > Height)
                        {
                            if (note.GetIsJudge() == false)
                            {
                                out << "Miss(out of screen) in:" << StartGameTime << "\n"
                                    << " key:" << note.GetKey() << "\n";
                                score.PlusMiss();
                            }
                        }
                        else
                        {
                            RenderQueue[i].push(note);
                        }
                    }
                }
                else // 长条
                {
                    note.Draw(Renderer, StripTexture);
                }
            }
        }
        // 判定渲染
    }
    // 添加note
    void AddNote(void)
    {
        if (NoteIndex < map.size() && StartGameTime >= (map[NoteIndex][0].GetStartTime()) - (NoteDownSpeed - 50))
        {
            for (auto &i : map[NoteIndex])
            {
                auto key = i.GetKey() - 1;
                auto x = (Widge / 2 - (TrackWidth * 2)) + (key * TrackWidth);
                RenderQueue[key].push(ShowNote(x, 0, TrackWidth, NoteHeight, i));
                out << "New note:" << i.GetStartTime() << " " << i.GetKey() << "Time: " << StartGameTime << "\n";
            }
            NoteIndex++;
        }
    }
    // 辅助函数 中途退出
    void Quit(void)
    {
        IsQuit = true;
        SDL_DestroyTexture(BackGround);
        SDL_DestroyTexture(NoteTexture);
        SDL_DestroyTexture(PerfectTexture);
        SDL_DestroyTexture(GoodTexture);
        SDL_DestroyTexture(BadTexture);
        SDL_DestroyTexture(MissTexture);
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyWindow(Window);
        TTF_CloseFont(Font);
        Mix_FreeMusic(Music);
        Mix_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        out.close();
        std::ofstream out("score.txt");
        out << score.GetPerfect() << "\n"
            << score.GetGood() << "\n"
            << score.GetBad() << "\n"
            << score.GetMiss() << "\n";
        out.close();
        std::exit(EXIT_SUCCESS);
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
        // 初始化渲染器
        Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        // 设置纹理线性过滤
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"))
        {
            throw(std::runtime_error("Warning: Linear texture filtering not enabled!"));
            std::exit(EXIT_FAILURE);
        }
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
        // 释放资源
        if (!IsQuit)
        {
            SDL_DestroyTexture(BackGround);
            SDL_DestroyTexture(NoteTexture);
            SDL_DestroyTexture(PerfectTexture);
            SDL_DestroyTexture(GoodTexture);
            SDL_DestroyTexture(BadTexture);
            SDL_DestroyTexture(MissTexture);
            SDL_DestroyRenderer(Renderer);
            SDL_DestroyWindow(Window);
            TTF_CloseFont(Font);
            Mix_FreeMusic(Music);
            Mix_Quit();
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
            out.close();
            out.open("score.txt");
            out << score.GetPerfect() << "\n"
                << score.GetGood() << "\n"
                << score.GetBad() << "\n"
                << score.GetMiss() << "\n";
            out.close();
        }
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
        NoteTexture = IMG_LoadTexture(Renderer, NoteFile.c_str());
        StripTexture = IMG_LoadTexture(Renderer, StripFile.c_str());
        PerfectTexture = IMG_LoadTexture(Renderer, ShowPerfectFile.c_str());
        GoodTexture = IMG_LoadTexture(Renderer, ShowGoodFile.c_str());
        BadTexture = IMG_LoadTexture(Renderer, ShowBadFile.c_str());
        MissTexture = IMG_LoadTexture(Renderer, ShowMissFile.c_str());
        ShowJudgePos.w = 256;
        ShowJudgePos.h = 82;
        ShowJudgePos.x = static_cast<float>(Widge) / 2 - ShowJudgePos.w / 2;
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
        RenderQueue = new std::queue<ShowNote>[TrackNum];
        StartTime = SDL_GetTicks();
        LastTime = StartTime;
        out.open("output.txt");
        NoteIndex = 0;
        out << ShowJudgePos.w << " " << ShowJudgePos.h << "\n";
        while (!IsQuit)
        {
            CurrentTime = SDL_GetTicks();
            out << "StartGameTime:" << StartGameTime << "\n";
            StartGameTime = CurrentTime - StartTime;
            DrawInterface();
            AddNote();
            Judge();
            Show();
            ShowJudge();
            SDL_RenderPresent(Renderer);
            LastTime = CurrentTime;
            if (StartGameTime > 10000)
            {
                break;
            }
        }
    }
};

#endif