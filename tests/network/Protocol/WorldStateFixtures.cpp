#include "WorldStateFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "WorldState.hpp"

namespace rtype::network::testing {

PlayerState somePlayer(std::uint8_t number) {
  return PlayerState{.playerNumber = number,
                     .shipEntityId = number,
                     .lives = SOME_LIVES,
                     .score = SOME_SCORE,
                     .invulnerable = false};
}

std::vector<PlayerState> fourPlayers() {
  return {somePlayer(1), somePlayer(2), somePlayer(3), somePlayer(4)};
}

std::vector<EntityState> someEntities(std::size_t count) {
  std::vector<EntityState> entities;
  entities.reserve(count);
  for (std::size_t index = 0; index < count; ++index) {
    entities.push_back(
        EntityState{.entityId = static_cast<std::uint32_t>(index + 1),
                    .entityType = WireEntityType::Bydo,
                    .x = static_cast<float>(index),
                    .y = SOME_ENTITY_Y});
  }
  return entities;
}

PlayerState invulnerablePlayer() {
  return PlayerState{.playerNumber = EXAMPLE_PLAYER_NUMBER,
                     .shipEntityId = EXAMPLE_PLAYER_SHIP_ID,
                     .lives = SOME_LIVES,
                     .score = SOME_SCORE,
                     .invulnerable = true};
}

WorldState oneShipState() {
  return WorldState{
      .tick = SOME_TICK,
      .players = {PlayerState{.playerNumber = EXAMPLE_PLAYER_NUMBER,
                              .shipEntityId = EXAMPLE_PLAYER_SHIP_ID,
                              .lives = SOME_LIVES,
                              .score = SOME_SCORE,
                              .invulnerable = false}},
      .entities = {EntityState{.entityId = EXAMPLE_ENTITY_ID,
                               .entityType = WireEntityType::PlayerShip,
                               .x = EXAMPLE_X,
                               .y = EXAMPLE_Y}}};
}

}  // namespace rtype::network::testing
