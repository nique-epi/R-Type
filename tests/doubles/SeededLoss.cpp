#include "SeededLoss.hpp"
#include <cstdint>

SeededLoss::SeededLoss(std::uint32_t seed, double lossRate)
    : generator_(seed), loss_(lossRate) {}

bool SeededLoss::losesNext() { return loss_(generator_); }
