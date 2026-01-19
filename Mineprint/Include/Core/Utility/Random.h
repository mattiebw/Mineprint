#pragma once

#include <random>

// The main random utility, that provides random number generation functions for various types based on a static mt19937 engine.
//
// Remember to call Random::Init() once at application startup to seed the random engine.
//
// This is provided as a general utility; it is not thread-safe, and should not be used if you want to have reproducible
// random sequences (in which case you should create your own random engine).
class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    NODISCARD FORCEINLINE static f32 Float()
    {
        return s_Distribution(s_RandomEngine);
    }

    NODISCARD FORCEINLINE static f32 Float(f32 min, f32 max)
    {
        return min + s_Distribution(s_RandomEngine) * (max - min);
    }

    NODISCARD FORCEINLINE static f64 Double()
    {
        return s_DoubleDistribution(s_RandomEngine);
    }

    NODISCARD FORCEINLINE static f64 Double(f64 min, f64 max)
    {
        return min + s_DoubleDistribution(s_RandomEngine) * (max - min);
    }

    NODISCARD FORCEINLINE static s32 Int()
    {
        return s_IntDistribution(s_RandomEngine);
    }

    NODISCARD FORCEINLINE static s32 Int(s32 min, s32 max)
    {
        return min + s_IntDistribution(s_RandomEngine) % (max - min);
    }

    NODISCARD FORCEINLINE static u32 UInt()
    {
        return s_UIntDistribution(s_RandomEngine);
    }

    NODISCARD FORCEINLINE static u32 UInt(u32 min, u32 max)
    {
        return min + s_UIntDistribution(s_RandomEngine) % (max - min);
    }

private:
    static std::mt19937                        s_RandomEngine;
    static std::uniform_real_distribution<f32> s_Distribution;
    static std::uniform_real_distribution<f64> s_DoubleDistribution;
    static std::uniform_int_distribution<s32>  s_IntDistribution;
    static std::uniform_int_distribution<u32>  s_UIntDistribution;
};
