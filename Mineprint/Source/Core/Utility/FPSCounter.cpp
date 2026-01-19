#include "mppch.h"

#include "Core/Utility/FPSCounter.h"

FPSCounter::FPSCounter()
{
    memset(&m_Samples, 0, sizeof(u16) * MaxSamples);
}

void FPSCounter::AddSample(u16 fps)
{
    m_Samples[m_SampleIndex] = fps;
    m_SampleIndex            = (m_SampleIndex + 1) % MaxSamples;
    m_Dirty                  = true;
}

void FPSCounter::RecalculateFPS()
{
    int fpsTotal = 0;
    u16 last     = 0;
    for (const u16 sample : m_Samples)
    {
        if (sample != 0)
        {
            fpsTotal += sample;
            last     = sample;
        }
        else
            fpsTotal += last;
    }
    m_FPS   = static_cast<u16>(fpsTotal / MaxSamples);
    m_Dirty = false;
}
