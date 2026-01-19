#pragma once

class FPSCounter
{
public:
    FPSCounter();

    void AddSample(u16 fps);

    NODISCARD FORCEINLINE u16 GetFPS()
    {
        if (m_Dirty) RecalculateFPS();
        return m_FPS;
    }

    NODISCARD FORCEINLINE f64 GetAverageFrameTimeMS() { return 1000.0 / GetFPS(); }

private:
    void RecalculateFPS();

    constexpr static u16 MaxSamples = 200;

    u16  m_Samples[MaxSamples];
    u16  m_FPS         = 0;
    u8   m_SampleIndex = 0;
    bool m_Dirty       = true;
};
