#include "BodyMasks.hpp"
#include <optional>
#include "CollisionMask.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "PlacedMask.hpp"
#include "Position.hpp"
#include "ShapeContacts.hpp"

namespace rtype::game {

std::optional<engine::PlacedMask> bodyMaskAt(
    engine::ComponentRegistry& components, engine::Entity body,
    const Position& position) {
  const auto* const mask = components.get<CollisionMask>(body);
  if (mask == nullptr || mask->pixels == nullptr) {
    return std::nullopt;
  }
  return placedMaskOf(position, *mask->pixels);
}

}  // namespace rtype::game
