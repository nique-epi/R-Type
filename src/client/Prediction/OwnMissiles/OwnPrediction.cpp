#include "OwnPrediction.hpp"
#include <cstdint>
#include <optional>
#include <vector>
#include "BlockingObstacles.hpp"
#include "ComponentRegistry.hpp"
#include "EntityReplicator.hpp"
#include "IOwnMissileListener.hpp"
#include "IPlayerInputLink.hpp"
#include "OwnMissilesParts.hpp"
#include "OwnShip.hpp"
#include "PlayerCommand.hpp"
#include "PredictedCommand.hpp"
#include "PredictedShipPlacement.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client {

OwnPrediction::OwnPrediction(std::uint8_t playerNumber, OwnMissilesParts parts)
    : playerNumber_(playerNumber),
      ship_(playerNumber),
      missiles_(parts),
      components_(parts.components) {}

void OwnPrediction::advance(engine::Duration elapsed, PlayerCommand command,
                            IPlayerInputLink& link) {
  missiles_.activate();
  collectBlockingObstacles(
      components_.get(),
      scrollLeadOf(scrollSpeed_, command.serverTick, command.shownTick),
      obstacles_);
  ship_.seeObstacles(obstacles_);
  const std::vector<PredictedCommand> commands =
      ship_.advance(elapsed, command, link);
  missiles_.follow(commands, firing_ && ship_.shipEntityId().has_value());
}

void OwnPrediction::apply(const network::WorldState& state) {
  const std::optional<std::uint32_t> shipBefore = ship_.shipEntityId();
  ship_.apply(state);
  const std::optional<OwnShip> own = ownShipOf(state, playerNumber_);
  if (ship_.shipEntityId() != shipBefore && own.has_value()) {
    missiles_.restart(own->acknowledgedInput, own->position);
  }
  if (const std::optional<std::uint32_t> acknowledged =
          acknowledgedInputOf(state, playerNumber_)) {
    missiles_.acknowledge(*acknowledged);
  }
}

void OwnPrediction::followScroll(float scrollSpeed) {
  scrollSpeed_ = scrollSpeed;
}

void OwnPrediction::ceaseFire() { firing_ = false; }

void OwnPrediction::place(const EntityReplicator& replicator,
                          engine::ComponentRegistry& components) {
  placePredictedShip(ship_, replicator, components);
  missiles_.touchEnemies();
}

IOwnMissileListener& OwnPrediction::missiles() { return missiles_; }

}  // namespace rtype::client
