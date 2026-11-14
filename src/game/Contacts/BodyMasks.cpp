#include "BodyMasks.hpp"
#include <optional>
#include "BarrelRoll.hpp"
#include "CollisionMask.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "PixelMask.hpp"
#include "PlacedMask.hpp"
#include "Position.hpp"
#include "ShapeContacts.hpp"
#include "ShipMotion.hpp"
#include "ShipRollMasks.hpp"

namespace rtype::game {

namespace {

/** @returns The mask of the ShipRollMasks of @p body at the point of the roll
 * its ShipMotion reached; none while it does not roll or has no such masks. */
const engine::PixelMask* rolledMaskOf(engine::ComponentRegistry& components,
                                      engine::Entity body) {
  const auto* const rolled = components.get<ShipRollMasks>(body);
  const auto* const motion = components.get<ShipMotion>(body);
  if (rolled == nullptr || rolled->masks == nullptr || motion == nullptr) {
    return nullptr;
  }
  const std::optional<float> share = rollShareOf(*motion);
  if (!share.has_value()) {
    return nullptr;
  }
  return &rolled->masks->shownAt(*share);
}

}  // namespace

std::optional<engine::PlacedMask> bodyMaskAt(
    engine::ComponentRegistry& components, engine::Entity body,
    const Position& position) {
  const auto* const mask = components.get<CollisionMask>(body);
  if (mask == nullptr || mask->pixels == nullptr) {
    return std::nullopt;
  }
  const engine::PixelMask* const rolled = rolledMaskOf(components, body);
  return placedMaskOf(position, rolled != nullptr ? *rolled : *mask->pixels);
}

void forgetMaskOf(engine::ComponentRegistry& components, engine::Entity body) {
  auto* const mask = components.get<CollisionMask>(body);
  if (mask != nullptr) {
    mask->pixels.reset();
  }
  auto* const rolled = components.get<ShipRollMasks>(body);
  if (rolled != nullptr) {
    rolled->masks.reset();
  }
}

}  // namespace rtype::game
