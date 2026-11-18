#pragma once

#include <cstdint>
#include <string>

namespace rtype::game {

/**
 * @brief Event published when what the players see of the level changes: its
 * biome, or its scroll starting, stopping or jumping back to a checkpoint.
 * It holds the whole of it: the biome, empty before the level names one, how
 * far the level scrolled, in logical units, and the speed of the scroll, in
 * logical units per second, 0 while paused, how far the level had scrolled
 * when the biome began, and the milliseconds left of the transition it plays,
 * 0 when it plays none.
 */
struct LevelChange {
  std::string biome;
  float scrollPosition{};
  float scrollSpeed{};
  float biomeSince{};
  std::uint16_t transitionLeft{};

  bool operator==(const LevelChange&) const = default;
};

}  // namespace rtype::game
