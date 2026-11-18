#pragma once

#include "PlayfieldConstants.hpp"

namespace rtype::game {

/**
 * @brief The rows a ship flies between, in logical units from the top edge of
 * the playfield: the whole play area, unless the biome of a level closes it
 * with a ceiling and a floor. A ship stops where its side meets one of them
 * (shipAfter()); a ship without it flies in the whole play area.
 */
struct PlayBand {
  float top{0.0F};
  float bottom{PLAY_AREA_HEIGHT};

  bool operator==(const PlayBand&) const = default;
};

}  // namespace rtype::game
