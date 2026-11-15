#pragma once

#include <vector>
#include "PixelMask.hpp"

namespace rtype::game {

/**
 * @brief Every collision mask a ship shows through its barrel roll, from
 * ROLL_THINNEST_ROWS rows to the height of its upright mask, on its back and
 * on its belly: row r of each copies the row sourceRowOf() gives of the
 * upright mask, the row the client draws there (rolledRowsOf()).
 *
 * Built once per sheet (RolledMaskCatalog), then shared by every ship of it.
 * The upright mask must be at least ROLL_THINNEST_ROWS rows tall, which the
 * catalog checks: a shorter one holds no mask, and shownAt() would throw.
 */
class RolledMasks {
 public:
  explicit RolledMasks(const engine::PixelMask& upright);

  /** @returns The mask a ship shows @p share of the way through its roll,
   * from 0 to 1. */
  [[nodiscard]] const engine::PixelMask& shownAt(float share) const;

 private:
  int height_{};
  std::vector<engine::PixelMask> back_;
  std::vector<engine::PixelMask> belly_;
};

}  // namespace rtype::game
