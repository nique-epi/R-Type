#include "PilotShipLooks.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include "ComponentRegistry.hpp"
#include "EffectTargets.hpp"
#include "Entity.hpp"
#include "PilotColor.hpp"
#include "PilotShips.hpp"
#include "PlayerLooks.hpp"
#include "ShipLook.hpp"
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
  }
}

}  // namespace rtype::client
