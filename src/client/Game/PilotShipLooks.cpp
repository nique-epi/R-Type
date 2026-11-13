#include "PilotShipLooks.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include "ComponentRegistry.hpp"
#include "EffectTargets.hpp"
#include "Entity.hpp"
#include "PilotColor.hpp"
#include "PilotHull.hpp"
#include "PilotShips.hpp"
#include "PlayerLooks.hpp"
#include "ShipHullSpriteConstants.hpp"
#include "ShipHulls.hpp"
#include "ShipLook.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "WornLook.hpp"

namespace rtype::client {

namespace {

/** @returns The look @p looks shows on the pilot of a pilot color; nothing
 * worn when it lists no pilot with its number. */
identifiers::ShipLook lookOfPilotColor(
    std::span<const network::PlayerLook> looks, int pilotColor) {
  const auto number =
      static_cast<std::uint8_t>(playerNumberOfPilotColor(pilotColor));
  const identifiers::ShipLook* const found = network::findLook(looks, number);
  return found == nullptr ? identifiers::ShipLook{} : *found;
}

/** @brief Makes @p ship show the hull of @p pilot: its sprite reads the hull
 * sheet and its animation plays the row of that hull and color. Nothing for
 * the standard hull, which keeps the frames of its entity sheet. */
void showHull(engine::ComponentRegistry& components, engine::Entity ship,
              PilotHull pilot) {
  const std::span<const game::SpriteFrame> frames = hullFramesOf(pilot);
  auto* const sprite = components.get<game::Sprite>(ship);
  if (frames.empty() || sprite == nullptr) {
    return;
  }
  if (sprite->assetId != SHIP_HULL_SHEET_ID) {
    sprite->assetId = SHIP_HULL_SHEET_ID;
  }
  sprite->frame = pilotShipFrameOf(pilot, sprite->frame).frame;
  if (auto* const animation = components.get<SpriteAnimation>(ship);
      animation != nullptr) {
    animation->animation.frames = frames;
  }
}

}  // namespace

void dressPilotShips(engine::ComponentRegistry& components,
                     std::span<const network::PlayerLook> looks) {
  const PilotShips found = pilotShipsOf(components);
  for (const engine::Entity ship : std::span(found.ships).first(found.count)) {
    const std::optional<int> pilotColor = pilotColorOf(components, ship);
    if (!pilotColor.has_value()) {
      continue;
    }
    const identifiers::ShipLook look = lookOfPilotColor(looks, *pilotColor);
    auto* const worn = components.get<WornLook>(ship);
    if (worn == nullptr) {
      components.add(ship, WornLook{.look = look});
    } else {
      worn->look = look;
    }
    showHull(components, ship,
             PilotHull{.hull = look.hull, .color = *pilotColor});
  }
}

}  // namespace rtype::client
