#include "RolledMasksFixtures.hpp"
#include <memory>
#include "CollisionMask.hpp"
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "MaskPixel.hpp"
#include "PixelMask.hpp"
#include "RolledMasks.hpp"
#include "ShipRollMasks.hpp"

namespace rtype::game::testing {

engine::PixelMask diagonalMask(int side) {
  engine::PixelMask mask{side, side};
  for (int row = 0; row < side; ++row) {
    mask.markDrawn(engine::MaskPixel{.x = row, .y = row});
  }
  return mask;
}

engine::Entity shipRollingOn(
    CollisionScene& scene,
    const std::shared_ptr<const engine::PixelMask>& upright) {
  const engine::Entity ship = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  scene.components.add(ship, CollisionMask{.pixels = upright});
  scene.components.add(
      ship,
      ShipRollMasks{.masks = std::make_shared<const RolledMasks>(*upright)});
  return ship;
}

}  // namespace rtype::game::testing
