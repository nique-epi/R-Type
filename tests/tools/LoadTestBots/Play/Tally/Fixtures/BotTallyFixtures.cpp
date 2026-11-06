#include "BotTallyFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "EntityMessage.hpp"
#include "GameEnded.hpp"
#include "WorldState.hpp"

namespace rtype::tools::testing {

client::EntityMessage worldStateAt(std::uint32_t tick) {
  return network::WorldState{.tick = tick, .players = {}, .entities = {}};
}

client::EntityMessage worldStateListing(std::uint32_t tick,
                                        std::size_t entityCount) {
  return network::WorldState{
      .tick = tick,
      .players = {},
      .entities = std::vector<network::EntityState>(entityCount)};
}

client::EntityMessage gameEnded() { return network::GameEnded{.players = {}}; }

}  // namespace rtype::tools::testing
