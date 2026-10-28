#include "PlayerShading.hpp"
#include <cstddef>
#include <optional>
#include "InterfaceColorConstants.hpp"
#include "PlayerColors.hpp"
#include "PlayerPalette.hpp"
#include "ShadeSwap.hpp"

namespace rtype::client {

PlayerShading shadingOf(const PlayerPalette& palette, std::size_t color) {
  const PlayerColors& sheet = PLAYER_COLORS.at(color);
  const PlayerColors& chosen = palette.at(color);
  if (chosen == sheet) {
    return {.shades = chosen, .recolor = std::nullopt};
  }
  return {.shades = chosen, .recolor = ShadeSwap{.from = sheet, .to = chosen}};
}

}  // namespace rtype::client
