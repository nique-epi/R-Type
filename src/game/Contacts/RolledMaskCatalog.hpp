#pragma once

#include <map>
#include <memory>
#include "PixelMask.hpp"
#include "RolledMasks.hpp"

namespace rtype::game {

/**
 * @brief The RolledMasks of every upright mask a world asked for, built the
 * first time each is asked for, then shared by every ship of its sheet.
 *
 * Keyed by the address of the upright mask, which the sheets own and which
 * outlives the world.
 */
class RolledMaskCatalog {
 public:
  /** @returns The masks @p upright shows through a barrel roll; none for a
   * mask shorter than ROLL_THINNEST_ROWS, which keeps its upright mask while
   * it rolls. */
  [[nodiscard]] std::shared_ptr<const RolledMasks> rolledOf(
      const engine::PixelMask& upright);

 private:
  std::map<const engine::PixelMask*, std::shared_ptr<const RolledMasks>>
      rolled_;
};

}  // namespace rtype::game
