#pragma once

#include "ComponentRegistry.hpp"
#include "EntityReplicator.hpp"
#include "OwnShipPredictor.hpp"

namespace rtype::client {

/**
 * @brief Puts the own ship of @p components where @p predictor says it is, in
 * place of where the last world state put it. Nothing while the predictor has
 * no ship, or the client has no entity for it.
 */
void placePredictedShip(const OwnShipPredictor& predictor,
                        const EntityReplicator& replicator,
                        engine::ComponentRegistry& components);

}  // namespace rtype::client
