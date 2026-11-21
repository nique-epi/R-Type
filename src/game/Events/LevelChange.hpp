#pragma once

#include <cstdint>
#include <string>
#include "PlayBand.hpp"

namespace rtype::game {

/**
 * @brief Event published when what the players see of the level changes: its
 * biome, or its scroll starting, stopping or jumping back to a checkpoint.
 * It holds the whole of it: the biome, empty before the level names one, how
 * far the level scrolled, in logical units, and the speed of the scroll, in
 * logical units per second, 0 while paused, how far the level had scrolled
 * when the biome began, the milliseconds left of the transition it plays,
 * 0 when it plays none, the rows the ships fly between in that biome, and the
 * times its team may still go back to its checkpoint after losing every life.
 */
struct LevelChange {
  std::string biome;
  float scrollPosition{};
  float scrollSpeed{};
  float biomeSince{};
  std::uint16_t transitionLeft{};
  PlayBand band{};
  std::uint8_t continuesLeft{};

  bool operator==(const LevelChange&) const = default;
};

}  // namespace rtype::game
