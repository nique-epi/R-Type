#include "SentStatesFixtures.hpp"
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SentState.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

SentState sentStateOf(std::uint32_t tick,
                      std::vector<std::uint16_t> sequences) {
  return SentState{
      .tick = tick,
      .sequences = std::move(sequences),
      .entities = std::make_shared<const std::vector<network::EntityState>>()};
}

network::PacketHeader acknowledging(std::uint16_t newest,
                                    std::uint32_t earlier) {
  return network::PacketHeader{.flags = network::ACKNOWLEDGES_FLAG,
                               .acknowledgedSequence = newest,
                               .acknowledgementBits = earlier};
}

}  // namespace rtype::server::testing
