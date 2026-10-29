#pragma once

#include <cstddef>
#include "EffectColorConstants.hpp"
#include "PilotDebrisRamps.hpp"
#include "PlayerColors.hpp"
#include "PlayerPalette.hpp"
#include "SpriteToneColorConstants.hpp"

namespace rtype::client {

/** @returns The colors the debris of the ship of each player color of
 * @p palette go through: FLASH_COLOR, then the light, main and dark shades of
 * that color, then SMOKE. constexpr, so the ramps of each palette are
 * constants the particles keep spans into. */
constexpr PilotDebrisRamps pilotDebrisRampsOf(const PlayerPalette& palette) {
  PilotDebrisRamps ramps{};
  for (std::size_t color = 0; color < palette.size(); ++color) {
    const PlayerColors& shades = palette[color];
    ramps[color] = {FLASH_COLOR, shades.light, shades.ship, shades.dark, SMOKE};
  }
  return ramps;
}

}  // namespace rtype::client
