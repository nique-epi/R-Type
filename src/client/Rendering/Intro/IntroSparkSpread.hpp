#pragma once

namespace rtype::client {

/** @brief How the sparks or the drops of a burst are drawn at random: their
 * speed, their life, how late they may start, and the chance that one is two
 * pixels large rather than one. */
struct IntroSparkSpread {
  float slowest{};
  float fastest{};
  float shortestLife{};
  float longestLife{};
  float latestStart{};
  float largeChance{};
};

}  // namespace rtype::client
