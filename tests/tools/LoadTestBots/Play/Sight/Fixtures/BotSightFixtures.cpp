#include "BotSightFixtures.hpp"
#include <cstdint>
#include "BotSightTestConstants.hpp"
#include "EntityMessage.hpp"
#include "GameEnded.hpp"
#include "WorldState.hpp"

namespace rtype::tools::testing {

client::EntityMessage stateOf(const SightScene& scene) {
  network::WorldState state{
      .tick = scene.tick,
      .players = {network::PlayerState{.playerNumber = SIGHT_PLAYER,
                                       .shipEntityId = SIGHT_SHIP_ID,
                                       .lives = scene.lives,
                                       .score = scene.score}},
      .entities = scene.entities};
  state.entities.push_back(network::EntityState{.entityId = SIGHT_SHIP_ID,
                                                .entityType = SHIP_TYPE,
                                                .x = SHIP_X,
                                                .y = SHIP_Y});
  return state;
}

network::EntityState entityAt(std::uint8_t type, float across) {
  return network::EntityState{
      .entityId = SEEN_ID, .entityType = type, .x = across, .y = SHIP_Y};
}

client::EntityMessage endOfGame() { return network::GameEnded{}; }

}  // namespace rtype::tools::testing
