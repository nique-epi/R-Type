#include "BotTallyFixtures.hpp"
#include <cstdint>
#include "EntityMessage.hpp"
#include "GameEnded.hpp"
#include "WorldState.hpp"

namespace rtype::tools::testing {

client::EntityMessage worldStateAt(std::uint32_t tick) {
  return network::WorldState{.tick = tick, .players = {}, .entities = {}};
}

client::EntityMessage gameEnded() { return network::GameEnded{.players = {}}; }

}  // namespace rtype::tools::testing
