#include "../includes/Game.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>

// Game初始化
Game::Game()
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
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
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

Game::~Game()
{
    // 释放资源
    if (!IsQuit)
    {
        SDL_DestroyTexture(BackGround);
        SDL_DestroyTexture(Note1Texture);
        SDL_DestroyTexture(PerfectTexture);
        SDL_DestroyTexture(GoodTexture);
        SDL_DestroyTexture(BadTexture);
        SDL_DestroyTexture(MissTexture);
        SDL_DestroyTexture(Note1Texture);
        SDL_DestroyTexture(Note2Texture);
        SDL_DestroyTexture(Note1LTexture);
        SDL_DestroyTexture(Note2LTexture);
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyWindow(Window);
        TTF_CloseFont(Font);
        TTF_CloseFont(ComboFont);
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

void Game::Init(std::string path)
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
    ComboFont = TTF_OpenFont(ComboFontFile.c_str(), 40);
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
    Note1Texture = IMG_LoadTexture(Renderer, Note1File.c_str());
    Note2Texture = IMG_LoadTexture(Renderer, Note2File.c_str());
    Note1LTexture = IMG_LoadTexture(Renderer, Note1LFile.c_str());
    Note2LTexture = IMG_LoadTexture(Renderer, Note2LFile.c_str());
    PerfectTexture = IMG_LoadTexture(Renderer, ShowPerfectFile.c_str());
    GoodTexture = IMG_LoadTexture(Renderer, ShowGoodFile.c_str());
    BadTexture = IMG_LoadTexture(Renderer, ShowBadFile.c_str());
    MissTexture = IMG_LoadTexture(Renderer, ShowMissFile.c_str());
    ShowJudgePos.w = 256;
    ShowJudgePos.h = 82;
    ShowJudgePos.x = static_cast<float>(Widge) / 2 - ShowJudgePos.w / 2;
}

void Game::ShowMenu(void)
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

void Game::Run(void)
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

    RenderQueue = new std::queue<ShowNote>[TrackNum];
    StartTime = SDL_GetTicks();
    LastTime = StartTime;
    out.open("output.txt");
    NoteIndex = 0;
    out << ShowJudgePos.w << " " << ShowJudgePos.h << "\n";
    Mix_PlayMusic(Music, 1);
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
        ShowCombo();
        SDL_RenderPresent(Renderer);
        LastTime = CurrentTime;
    }
}

void Game::Quit(void)
{
    IsQuit = true;
    SDL_DestroyTexture(BackGround);
    SDL_DestroyTexture(Note1Texture);
    SDL_DestroyTexture(PerfectTexture);
    SDL_DestroyTexture(GoodTexture);
    SDL_DestroyTexture(BadTexture);
    SDL_DestroyTexture(MissTexture);
    SDL_DestroyTexture(Note1Texture);
    SDL_DestroyTexture(Note2Texture);
    SDL_DestroyTexture(Note1LTexture);
    SDL_DestroyTexture(Note2LTexture);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    TTF_CloseFont(Font);
    Mix_FreeMusic(Music);
    TTF_CloseFont(ComboFont);
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

void Game::AddNote(void)
{
    if (NoteIndex < map.size() && StartGameTime >= (map[NoteIndex][0].GetStartTime()) - (NoteDownSpeed - 10))
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

void Game::Show(void)
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
                    if ((i & 1) == 0)
                    {
                        note.Draw(Renderer, Note1Texture);
                    }
                    else
                    {
                        note.Draw(Renderer, Note2Texture);
                    }
                    auto Distance = Speed * (CurrentTime - LastTime);
                    note.PlusRectYPos(Distance);
                    if (note.GetYPos() > Height)
                    {
                        if (note.GetIsJudge() == false)
                        {
                            out << "Miss(out of screen) in:" << StartGameTime << "\n"
                                << " key:" << note.GetKey() << "\n";
                            score.PlusMiss();
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
                    }
                    else
                    {
                        RenderQueue[i].push(note);
                    }
                }
            }
            else // 长条
            {
                if (StartGameTime >= (note.GetEndTime() - (NoteDownSpeed - 10)))
                {
                    out << "note end here:" << StartGameTime << "\n";
                    note.SetIsEnd(true);
                }
                if ((i & 1) == 0)
                {
                    note.Draw(Renderer, Note1LTexture);
                }
                else
                {
                    note.Draw(Renderer, Note2LTexture);
                }
                auto Distance = Speed * (CurrentTime - LastTime);
                if (note.GetIsEnd()) // 长条尾部已经进入屏幕（长条长度固定）
                {
                    note.PlusRectYPos(Distance);
                    if ((note.GetYPos() + note.GetRectH()) > JudgeLine.y) // 长条头部已经越过判定线
                    {
                        note.MinusRectH(static_cast<float>((note.GetYPos() + note.GetRectH() - JudgeLine.y)));
                    }
                }
                else // 长条尾部仍未进入屏幕（更新长条长度）
                {
                    note.PlusRectHPos(Distance);
                    if ((note.GetYPos() + note.GetRectH()) > JudgeLine.y) // 长条头部已经越过判定线
                    {
                        note.MinusRectH(static_cast<float>((note.GetYPos() + note.GetRectH() - JudgeLine.y)));
                    }
                }
                if (note.GetYPos() > Height) // 长条尾部过判定线
                {
                    if (note.GetIsJudge() == false)
                    {
                        out << "Miss(out of screen) in:" << StartGameTime << "\n"
                            << "    key:" << note.GetKey() << "\n";
                        score.PlusMiss();
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
                }
                else
                {
                    RenderQueue[i].push(note);
                }
            }
        }
    }
}

void Game::ShowCombo(void)
{
    auto Combo = score.GetCombo();
    if (Combo > 0)
    {
        auto ComboSurface = TTF_RenderText_Blended(ComboFont, std::to_string(Combo).c_str(), ComboColor);
        auto ComboTexture = SDL_CreateTextureFromSurface(Renderer, ComboSurface);
        SDL_Rect ComboPos;
        SDL_QueryTexture(ComboTexture, nullptr, nullptr, &ComboPos.w, &ComboPos.h);
        ComboPos.x = (Widge - ComboPos.w) / 2;
        ComboPos.y = ShowJudgePos.y - 50;
        SDL_RenderCopy(Renderer, ComboTexture, nullptr, &ComboPos);
        SDL_FreeSurface(ComboSurface);
        SDL_DestroyTexture(ComboTexture);
    }
}
void Game::ShowJudge(void)
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

void Game::JudgeNoteStart(ShowNote &note)
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
        }
    }
    else // late
    {
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
    }
}
void Game::JudgeStripStart(ShowNote &note)
{
    auto diff = StartGameTime - note.GetStartTime();
    if (diff < 0) // fast
    {
        diff = std::abs(diff);
        if (diff < PerfectTiming)
        {
            score.PlusPerfect();
            out << " res(Strip Start): perfect(early)"
                << "\n";
            note.SetIsStartJudge(true);
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
            out << " res(Strip Start): good(early)"
                << "\n";
            note.SetIsStartJudge(true);
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
            out << " res(Strip Start): bad(early)"
                << "\n";
            note.SetIsStartJudge(true);
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
    }
    else // late
    {
        if (diff < PerfectTiming)
        {
            out << " res(Strip Start): perfect(late)"
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
            out << " res(Strip Start): good(late)"
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
            out << " res(Strip Start): bad(late)"
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
            out << " res(Strip Start): miss(late)"
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
        note.SetIsStartJudge(true);
    }
}

void Game::JudgeStripMid(ShowNote &note)
{
    auto LastJudgeTime = note.GetLastJudgeTime();
    if (Event.type == SDL_KEYUP)
    {
        score.PlusMiss();
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
    else
    {
        if (CurrentTime - LastJudgeTime >= 10)
        {
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
            note.SetLastJudgeTime(CurrentTime);
        }
    }
}

void Game::JudgeStripEnd(ShowNote &note) // 长条尾部判定
{
    auto diff = StartGameTime - note.GetEndTime();
    if (diff < 0) // fast
    {
        diff = std::abs(diff);
        if (diff < PerfectTiming)
        {
            score.PlusPerfect();
            out << " res(Strip End): perfect(early)"
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
        }
        else if (diff < GoodTiming)
        {
            score.PlusGood();
            out << " res(Strip End): good(early)"
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
        }
        else if (diff < BadTiming)
        {
            score.PlusBad();
            out << " res(Strip End): bad(early)"
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
        }
        else if (diff < MissTiming)
        {
            score.PlusMiss();
            out << " res(Strip End): miss(early)"
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
            NowShowTexture = MissTexture;
        }
    }
    else // late
    {
        if (diff < PerfectTiming)
        {
            out << " res(Strip End): perfect(late)"
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
            out << " res(Strip End): good(late)"
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
        else
        {
            score.PlusBad();
            out << " res(Strip End): bad(late)"
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
        note.SetIsJudge(true);
    }
}

void Game::Judge(void)
{
    if (SDL_PollEvent(&Event))
    {
        for (std::int32_t i = 0; i < TrackNum; i++)
        {
            auto &note = RenderQueue[i].front();
            auto ReqKey = KeyCodeForFour[i];

            if (Event.type == SDL_QUIT)
            {
                Quit();
            }
            else if (note.GetType() == NoteType::Single) // 单键
            {
                if (Event.type == SDL_KEYDOWN)
                {
                    out << "time after start:" << StartGameTime << "  ";
                    out << "in:" << i << " key is :" << Event.key.keysym.sym << "req key is:" << ReqKey << "\n";
                    if (Event.key.keysym.sym == ReqKey)
                    {
                        JudgeNoteStart(note);
                        break;
                    }
                }
            }
            else if (note.GetType() == NoteType::Strip) // 长条
            {
                if (Event.key.keysym.sym == ReqKey)
                {
                    if (note.GetIsStartJudge()) // 头部已经进判定
                    {
                        if (note.GetEndTime() - StartGameTime <= MissTiming && Event.type == SDL_KEYUP) // 尾判开始
                        {
                            JudgeStripEnd(note);
                            break;
                        }
                        else // 长条内部判定
                        {
                            JudgeStripMid(note);
                            break;
                        }
                    }
                    else // 头部未进判定则对头部进行判定
                    {
                        JudgeStripStart(note);
                        break;
                    }
                }
            }
        }
    }
}

void Game::DrawInterface(void)
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