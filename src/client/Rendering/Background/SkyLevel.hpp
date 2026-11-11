#pragma once

#include <string>

namespace rtype::client {

/**
 * @brief What the sky shows of the level a game plays: the biome whose decor
 * it draws, empty for none, how far the level scrolled, in logical units of
 * the playfield, and how fast it scrolls, in logical units per second, 0
 * while its scroll is paused.
 */
struct SkyLevel {
  std::string biome;
  float scrollPosition{};
  float scrollSpeed{};

  bool operator==(const SkyLevel&) const = default;
};

}  // namespace rtype::client
