#include "OwnPredictionFixture.hpp"
#include <cstddef>
#include "ActionSet.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "OwnPrediction.hpp"
#include "PlayerAction.hpp"
#include "PlayerCommand.hpp"
#include "Position.hpp"
#include "PredictionFixtures.hpp"
#include "PredictionTestConstants.hpp"
#include "Sprite.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

OwnPredictionFixture::OwnPredictionFixture() {
  prediction_.apply(stateWithShip(OWN_PLAYER_NUMBER, OWN_SHIP_ID, START, 0));
}

void OwnPredictionFixture::shootOneTick() {
  ActionSet held;
  held.add(PlayerAction::Shoot);
  prediction_.advance(engine::SIMULATION_TICK_DURATION,
                      PlayerCommand{.held = held}, link_);
}

void OwnPredictionFixture::apply(const network::WorldState& state) {
  prediction_.apply(state);
}

std::size_t OwnPredictionFixture::drawnMissileCount() {
  std::size_t count = 0;
  components_.forEach<game::EntityType, game::Position>(
      [this, &count](engine::Entity entity, const game::EntityType& side,
                     const game::Position&) {
        if (side == game::EntityType::PlayerMissile &&
            components_.has<game::Sprite>(entity)) {
          ++count;
        }
      });
  return count;
}

OwnPrediction& OwnPredictionFixture::prediction() { return prediction_; }

}  // namespace rtype::client::testing
