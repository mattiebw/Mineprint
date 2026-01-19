#pragma once

#include <SDL3/SDL_timer.h>

class Stopwatch
{
public:
    Stopwatch()
    {
        Restart();
    }

    FORCEINLINE void Restart()
    {
        m_End   = 0;
        m_Begin = SDL_GetPerformanceCounter();
    }

    FORCEINLINE void End()
    {
        m_End = SDL_GetPerformanceCounter();
    }

    NODISCARD FORCEINLINE f64 GetElapsedMilliseconds() const
    {
        u64 end = m_End;
        if (end == 0)
            end = SDL_GetPerformanceCounter();
        return static_cast<f64>((end - m_Begin) * 1000) / static_cast<f64>(SDL_GetPerformanceFrequency());
    }

    NODISCARD FORCEINLINE f64 GetElapsedSeconds() const
    {
        u64 end = m_End;
        if (end == 0)
            end = SDL_GetPerformanceCounter();
        return static_cast<f64>(end - m_Begin) / static_cast<f64>(SDL_GetPerformanceFrequency());
    }

    NODISCARD FORCEINLINE u64 GetElapsedTicks() const
    {
        u64 end = m_End;
        if (end == 0)
            end = SDL_GetPerformanceCounter();
        return end - m_Begin;
    }

    NODISCARD FORCEINLINE u64 GetElapsedMicroseconds() const
    {
        u64 end = m_End;
        if (end == 0)
            end = SDL_GetPerformanceCounter();
        return (end - m_Begin) * 1000000 / SDL_GetPerformanceFrequency();
    }

private:
    u64 m_Begin, m_End;
};

template<typename T>
concept HasOnEnd = requires(std::string_view name, u64 ticks, int depth) {
    { T::OnEnd(name, ticks, depth) } -> std::same_as<void>;
};

struct LogMSOnEndOfScope
{
    static void OnEnd(std::string_view name, u64 elapsedTicks, int depth)
    {
        MP_INFO("{: >{}}{}: {:.3f}ms", "", depth, name, (static_cast<double>(elapsedTicks) / static_cast<double>(SDL_GetPerformanceFrequency())) * 1000);
    }
};

template <HasOnEnd EndHandler = LogMSOnEndOfScope>
struct LoggingScopedTimer
{
    LoggingScopedTimer(std::string_view name)
        : m_Name(name), m_Start(SDL_GetPerformanceCounter())
    {
        m_TimerDepth++;
    }

    ~LoggingScopedTimer()
    {
        u64 diff = SDL_GetPerformanceCounter() - m_Start;
        EndHandler::OnEnd(m_Name, diff, m_TimerDepth);
        m_TimerDepth--;
    }

private:
    static s32 m_TimerDepth;

    std::string_view m_Name;
    u64 m_Start;
};

// Basic macro to create a logging scoped timer without having to manually declare a unique variable name.
#define TOKEN_PASTE(x, y) x##y
#define CAT(x,y) TOKEN_PASTE(x,y)
#define LOG_SCOPE_TIMER(x) auto CAT(timer, __LINE__) = LoggingScopedTimer(x)
