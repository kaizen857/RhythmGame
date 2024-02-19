#ifndef _NOTE_HPP_
#define _NOTE_HPP_

#include <cstdint>
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
    uint16_t Perfect;
    uint16_t Good;
    uint16_t Bad;
    uint16_t Miss;

public:
    Counter(int per = 0, int good = 0, int bad = 0, int miss = 0) :
        Perfect(per), Good(good), Bad(bad), Miss(miss){};

    int GetPerfect(void) const
    {
        return Perfect;
    }
    int GetGood(void) const
    {
        return Good;
    }
    int GetBad(void) const
    {
        return Bad;
    }
    int GetMiss(void) const
    {
        return Miss;
    }

    void PlusPerfect(void)
    {
        ++Perfect;
    }
    void PlusGood(void)
    {
        ++Good;
    }
    void PlusBad(void)
    {
        ++Bad;
    }
    void PlusMiss(void)
    {
        ++Miss;
    }

    void Resect(void)
    {
        Perfect = Good = Bad = Miss = 0;
    }
};

class ShowNote
{
private:
    Note note;
    SDL_Rect Rect;
    bool IsJudge = false;

public:
    ShowNote(int x, int y, int w, int h, Note note_) :
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

    void SetRectYPos(int y)
    {
        Rect.y = y;
    }

    void PlusRectYPos(int y)
    {
        Rect.y += y;
    }

    // getter
    bool GetIsJudge(void) const
    {
        return IsJudge;
    }

    int GetYPos(void) const
    {
        return Rect.y;
    }
    // 获取note开始时间
    std::time_t GetStartTime(void) const
    {
        return note.GetStartTime();
    }
    // 获取note结束时间
    std::time_t GetEndTime(void) const
    {
        return note.GetEndTime();
    }
    // 获取note轨道位置
    int GetKey() const
    {
        return note.GetKey();
    }
    // 获取note类型
    NoteType GetType() const
    {
        return note.GetType();
    }

    inline void Draw(SDL_Renderer *renderer, SDL_Texture *texture) const
    {
        SDL_RenderCopy(renderer, texture, nullptr, &Rect);
    }
};

#endif