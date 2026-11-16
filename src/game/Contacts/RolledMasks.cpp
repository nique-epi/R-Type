#include "RolledMasks.hpp"
#include <cstddef>
#include "BarrelRoll.hpp"
#include "MaskPixel.hpp"
#include "PixelMask.hpp"
#include "RollMoment.hpp"
#include "RolledRows.hpp"
#include "ShipControlConstants.hpp"

namespace rtype::game {

namespace {

/** @returns The mask @p upright shows as @p rolled: one row for each row the
 * roll shows, copied from the row of @p upright drawn there. */
engine::PixelMask rolledMaskOf(const engine::PixelMask& upright,
                               RolledRows rolled) {
  engine::PixelMask shown{upright.width(), rolled.count};
  for (int row = 0; row < rolled.count; ++row) {
    const int source = sourceRowOf(rolled, row);
    for (int column = 0; column < upright.width(); ++column) {
      if (upright.isDrawn(engine::MaskPixel{.x = column, .y = source})) {
        shown.markDrawn(engine::MaskPixel{.x = column, .y = row});
      }
    }
  }
  return shown;
}

}  // namespace

RolledMasks::RolledMasks(const engine::PixelMask& upright)
    : height_(upright.height()) {
  for (int count = ROLL_THINNEST_ROWS; count <= height_; ++count) {
    back_.push_back(rolledMaskOf(
        upright,
        RolledRows{.height = height_, .count = count, .belly = false}));
    belly_.push_back(rolledMaskOf(
        upright, RolledRows{.height = height_, .count = count, .belly = true}));
  }
}

const engine::PixelMask& RolledMasks::shownAt(float share) const {
  const RolledRows rolled =
      rolledRowsOf(RollMoment{.frameHeight = height_, .share = share});
  const auto index =
      static_cast<std::size_t>(rolled.count - ROLL_THINNEST_ROWS);
  return rolled.belly ? belly_.at(index) : back_.at(index);
}

}  // namespace rtype::game
