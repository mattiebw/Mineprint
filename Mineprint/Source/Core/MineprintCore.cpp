#include "mppch.h"

// The disabled warning is for an unseeded engine, which will return the same sequence each time, but we seed it in Random::Init().
std::mt19937    Random::s_RandomEngine;   // NOLINT(cert-msc51-cpp)
std::mt19937_64 Random::s_RandomEngine64; // NOLINT(cert-msc51-cpp)
