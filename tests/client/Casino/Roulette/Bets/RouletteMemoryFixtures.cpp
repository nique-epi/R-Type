#include "RouletteMemoryFixtures.hpp"
#include <cstdint>
#include "RouletteRound.hpp"

namespace rtype::client::testing {

network::RouletteRound roundAt(std::uint16_t number,
                               network::RoulettePhase phase,
                               std::uint8_t winningNumber) {
  return network::RouletteRound{.round = number,
                                .phase = phase,
                                .ticksLeft = 0,
                                .winningNumber = winningNumber,
                                .seats = {}};
}

}  // namespace rtype::client::testing
