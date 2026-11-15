#include "RolledMaskCatalog.hpp"
#include <memory>
#include "PixelMask.hpp"
#include "RolledMasks.hpp"
#include "ShipControlConstants.hpp"

namespace rtype::game {

std::shared_ptr<const RolledMasks> RolledMaskCatalog::rolledOf(
    const engine::PixelMask& upright) {
  if (upright.height() < ROLL_THINNEST_ROWS) {
    return nullptr;
  }
  std::shared_ptr<const RolledMasks>& rolled = rolled_[&upright];
  if (rolled == nullptr) {
    rolled = std::make_shared<const RolledMasks>(upright);
  }
  return rolled;
}

}  // namespace rtype::game
