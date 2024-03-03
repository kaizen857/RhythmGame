#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
// #include <chrono>
#include <queue>
#include <cstdint>
#include <fstream>
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
    const std::int16_t Widge = 1920;
    const std::int16_t Height = 1080;
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
    // 轨道宽度
    const Uint16 TrackWidth = 100;
    // note高度
    const Uint16 NoteHeight = 50;
    // 轨道数
    const Uint8 TrackNum = 4;
    // 判定线位置
    SDL_FRect JudgeLine{static_cast<float>(Widge) / 2.0f - 200.0f, static_cast<float>(Height) - 80.0f, static_cast<float>(TrackWidth *TrackNum), 15.0f};
    // 显示判定的位置
    SDL_FRect DisplayJudge{static_cast<float>(Widge) / 2.0f, static_cast<float>(Height) / 2.0f - 100.0f, 200.0f, 50.0f};
    // 距离判定显示后的时间
    Uint32 DisplayTime = 0;
    // 判定显示的最大时间
    const Uint32 MaxDisplayTime = 1000;

    // 音符下落时间(单位ms)
    const Uint32 NoteDownSpeed = 400;
    // 音符下落速度(单位pix/ms)
    const double Speed = static_cast<double>(JudgeLine.y) / static_cast<double>(NoteDownSpeed);
    // 准确度判定(单位：ms)
    const Uint8 PerfectTiming = 50;
    const Uint8 GoodTiming = 100;
    const Uint8 BadTiming = 150;
    const Uint8 MissTiming = 200;
    // 键位
    SDL_KeyCode KeyCodeForFour[4] = {SDLK_a, SDLK_s, SDLK_k, SDLK_l};
    // 音量
    const Uint8 MusicVolume = 64;

    // 游戏note材质
    const std::string Note1File = "./res/mania/mania_note1.png";
    const std::string Note2File = "./res/mania/mania_note2.png";
    const std::string Note1LFile = "./res/mania/mania_note1L.png";
    const std::string Note2LFile = "./res/mania/mania_note2L.png";
    SDL_Texture *Note1Texture;
    SDL_Texture *Note2Texture;
    SDL_Texture *Note1LTexture;
    SDL_Texture *Note2LTexture;
    const std::string StripFile = "./res/mania/B.png";
    // 游戏判定相关材质
    const std::string ShowPerfectFile = "./res/mania/mania_hitPerfect@2x.png";
    const std::string ShowGoodFile = "./res/mania/mania_hitGood@2x.png";
    const std::string ShowBadFile = "./res/mania/mania_hitBad@2x.png";
    const std::string ShowMissFile = "./res/mania/mania_hitMiss@2x.png";
    SDL_Texture *PerfectTexture;
    SDL_Texture *GoodTexture;
    SDL_Texture *BadTexture;
    SDL_Texture *MissTexture;

    // 游戏分数字体
    const std::string ComboFontFile = "./res/LCALLIG.TTF";
    TTF_Font *ComboFont;
    SDL_Color ComboColor{255, 255, 255};

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
    void DrawInterface(void);

    // 判定
    void Judge(void);

    // 普通note判定
    void JudgeNoteStart(ShowNote &note);
    // 长条头部判定
    void JudgeStripStart(ShowNote &note);
    // 长条中间判定
    void JudgeStripMid(ShowNote &note);
    // 长条尾部判定
    void JudgeStripEnd(ShowNote &note);

    // 显示判定
    void ShowJudge(void);

    // 渲染
    void Show(void);

    // 添加note
    void AddNote(void);
    // 辅助函数 中途退出
    void Quit(void);

    // 显示连击数
    void ShowCombo(void);

public:
    Game();

    ~Game();

    void Init(std::string path);

    void ShowMenu(void);

    void Run(); // 下落速度400ms
};

#endif