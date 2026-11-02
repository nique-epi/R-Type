#include "PredictedShipPlacement.hpp"
#include <cstdint>
#include <optional>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityReplicator.hpp"
#include "OwnShipPredictor.hpp"
#include "Position.hpp"

namespace rtype::client {

void placePredictedShip(const OwnShipPredictor& predictor,
                        const EntityReplicator& replicator,
                        engine::ComponentRegistry& components) {
  const std::optional<std::uint32_t> networkId = predictor.shipEntityId();
  if (!networkId.has_value()) {
    return;
  }
  const std::optional<engine::Entity> ship =
      replicator.shipEntityOf(*networkId);
  if (!ship.has_value()) {
    return;
  }
  if (auto* const position = components.get<game::Position>(*ship)) {
    *position = predictor.position();
  }
}

}  // namespace rtype::client
