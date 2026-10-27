#pragma once

#include <cstddef>
#include <optional>
#include "PlayerColors.hpp"
#include "PlayerPalette.hpp"
#include "ShadeSwap.hpp"

namespace rtype::client {

/** @brief How a player color is painted in the palette the player chose: its
 * shades, and the swap a frame of the sheets needs to show them, none when
 * the palette keeps the shades of the sheets. */
struct PlayerShading {
  PlayerColors shades;
  std::optional<ShadeSwap> recolor;
};

/**
 * @returns How @p palette paints player color @p color, 0 to
 * PLAYER_COLOR_COUNT - 1.
 * @throws std::out_of_range (from std::array::at) for a color outside the
 * palette; every caller passes the color of a pilot it already checked.
 */
[[nodiscard]] PlayerShading shadingOf(const PlayerPalette& palette,
                                      std::size_t color);

}  // namespace rtype::client
