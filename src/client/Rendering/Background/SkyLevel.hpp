#pragma once

#include <string>

namespace rtype::client {

/**
 * @brief What the sky shows of the level a game plays: the biome whose decor
 * it draws, empty for none, how far the level scrolled, in logical units of
 * the playfield, how fast it scrolls, in logical units per second, 0 while
 * its scroll is paused, and how far it had scrolled when that biome began,
 * where its maps start sliding in from the right edge.
 */
struct SkyLevel {
  std::string biome;
  float scrollPosition{};
  float scrollSpeed{};
  float biomeSince{};

  bool operator==(const SkyLevel&) const = default;
};

}  // namespace rtype::client
