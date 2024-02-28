#ifndef _NOTE_HPP_
#define _NOTE_HPP_

#include <ctime>
#include <SDL2/SDL.h>

enum NoteType
{
    Single,
    Strip
};

class Note
{
private:
    std::time_t StartTime;
    std::time_t EndTime;
    int Key;
    NoteType Type;

public:
    Note(std::time_t startTime, std::time_t endTime, int key, NoteType type) :
        StartTime(startTime), EndTime(endTime), Key(key), Type(type)
    {
    }

    // Getters
    std::time_t GetStartTime() const
    {
        return StartTime;
    }
    std::time_t GetEndTime() const
    {
        return EndTime;
    }

    int GetKey() const
    {
        return Key;
    }
    NoteType GetType() const
    {
        return Type;
    }

    // setter
    void SetStartTime(std::time_t startTime)
    {
        StartTime = startTime;
    }

    void SetEndTime(std::time_t endTime)
    {
        EndTime = endTime;
    }

    void SetKey(int key)
    {
        Key = key;
    }

    void SetType(NoteType type)
    {
        Type = type;
    }

    void SetType(int type)
    {
        if (type == 0)
        {
            Type = Single;
        }
        else
        {
            Type = Strip;
        }
    }
};

class Counter
{
private:
    Uint16 Perfect = 0;
    Uint16 Good = 0;
    Uint16 Bad = 0;
    Uint16 Miss = 0;
    Uint16 Combo = 0;

public:
    Counter(int per = 0, int good = 0, int bad = 0, int miss = 0) :
        Perfect(per), Good(good), Bad(bad), Miss(miss){};

    auto GetPerfect(void) const
    {
        return Perfect;
    }
    auto GetGood(void) const
    {
        return Good;
    }
    auto GetBad(void) const
    {
        return Bad;
    }
    auto GetMiss(void) const
    {
        return Miss;
    }
    auto GetCombo(void) const
    {
        return Combo;
    }

    void PlusPerfect(void)
    {
        ++Perfect;
        ++Combo;
    }
    void PlusGood(void)
    {
        ++Good;
        ++Combo;
    }
    void PlusBad(void)
    {
        ++Bad;
        ++Combo;
    }
    void PlusMiss(void)
    {
        ++Miss;
        Combo = 0;
    }

    void Resect(void)
    {
        Perfect = Good = Bad = Miss = Combo = 0;
    }
};

class ShowNote
{
private:
    Note note;
    SDL_FRect Rect;
    bool IsJudge = false;

public:
    ShowNote(float x, float y, float w, float h, Note note_) :
        note(note_)
    {
        Rect.x = x;
        Rect.y = y;
        Rect.w = w;
        Rect.h = h;
    }
    // setter
    void SetIsJudge(bool isJudge)
    {
        IsJudge = isJudge;
    }

    void SetRectYPos(float y)
    {
        Rect.y = y;
    }

    void PlusRectYPos(float y)
    {
        Rect.y += y;
    }

    void PlusRectHPos(float h)
    {
        Rect.h += h;
    }

    // getter
    bool GetIsJudge(void) const
    {
        return IsJudge;
    }

    auto GetYPos(void) const
    {
        return Rect.y;
    }
    // 获取note开始时间
    auto GetStartTime(void) const
    {
        return note.GetStartTime();
    }
    // 获取note结束时间
    auto GetEndTime(void) const
    {
        return note.GetEndTime();
    }
    // 获取note轨道位置
    auto GetKey() const
    {
        return note.GetKey();
    }
    // 获取note类型
    auto GetType() const
    {
        return note.GetType();
    }

    inline void Draw(SDL_Renderer *renderer, SDL_Texture *texture) const
    {
        SDL_RenderCopyF(renderer, texture, nullptr, &Rect);
    }
};

#endif