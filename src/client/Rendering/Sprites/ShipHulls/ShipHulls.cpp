#include "ShipHulls.hpp"
#include <cstddef>
#include <optional>
#include <span>
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "PilotColor.hpp"
#include "PilotHull.hpp"
#include "ShipHullSpriteConstants.hpp"
#include "ShipLookNumbers.hpp"
#include "ShipSheetFrame.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

namespace {

/** @returns The row of the hull sheet @p pilot flies; nothing for the
 * standard hull, a number that names no hull, or a color no pilot has. */
std::optional<std::size_t> hullRowOf(PilotHull pilot) {
  if (!identifiers::isHullNumber(pilot.hull) || !isPilotColor(pilot.color)) {
    return std::nullopt;
  }
  return (identifiers::hullIndexOf(pilot.hull) *
          static_cast<std::size_t>(PLAYER_COLOR_COUNT)) +
         static_cast<std::size_t>(pilot.color);
}

}  // namespace

ShipSheetFrame pilotShipFrameOf(PilotHull pilot, game::SpriteFrame frame) {
  const std::optional<std::size_t> row = hullRowOf(pilot);
  if (!row.has_value()) {
    return ShipSheetFrame{.sheet = ENTITY_SHEET_ID,
                          .frame = inPlayerColorRow(frame, pilot.color)};
  }
  frame.top = SHIP_HULL_ROWS.at(*row).front().top;
  return ShipSheetFrame{.sheet = SHIP_HULL_SHEET_ID, .frame = frame};
}

std::span<const game::SpriteFrame> hullFramesOf(PilotHull pilot) {
  const std::optional<std::size_t> row = hullRowOf(pilot);
  if (!row.has_value()) {
    return {};
  }
  return SHIP_HULL_ROWS.at(*row);
}

}  // namespace rtype::client
