#pragma once

#include "ComponentRegistry.hpp"
#include "EntityReplicator.hpp"
#include "OwnShipPredictor.hpp"

namespace rtype::client {

/**
 * @brief Puts the own ship of @p components where @p predictor says it is, in
 * place of where the last world state put it, and draws its barrel roll as
 * far as the prediction says: its ShipFlight follows the predicted roll, as
 * the server lays its collision mask, not the roll a world state reports a
 * round trip later. Call it after ShipFlightSystem in the frame, so the roll
 * it writes is the one drawn. Nothing while the predictor has no ship, or the
 * client has no entity for it.
 */
void placePredictedShip(const OwnShipPredictor& predictor,
                        const EntityReplicator& replicator,
                        engine::ComponentRegistry& components);

}  // namespace rtype::client
