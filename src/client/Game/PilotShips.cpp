#include "PilotShips.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "PilotShip.hpp"
#include "Sprite.hpp"

namespace rtype::client {

PilotShips pilotShipsOf(engine::ComponentRegistry& components) {
  PilotShips found;
  components.forEach<PilotShip, game::Sprite>(
      [&found](engine::Entity ship, const PilotShip&, const game::Sprite&) {
        if (found.count < found.ships.size()) {
          found.ships.at(found.count) = ship;
          ++found.count;
        }
      });
  return found;
}

}  // namespace rtype::client
