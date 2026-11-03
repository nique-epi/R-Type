#include "StateRateMeasurementFixtures.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <vector>
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "StateRateMeasurementTestConstants.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

std::size_t bytesSentTo(std::span<const network::OutgoingDatagram> sent,
                        const network::Endpoint& player) {
  std::size_t bytes = 0;
  for (const network::OutgoingDatagram& datagram : sentTo(sent, player)) {
    bytes += datagram.payload.size();
  }
  return bytes;
}

std::vector<std::uint32_t> ticksSentTo(
    std::span<const network::OutgoingDatagram> sent,
    const network::Endpoint& player) {
  std::vector<std::uint32_t> ticks;
  for (const network::OutgoingDatagram& datagram : sentTo(sent, player)) {
    ticks.push_back(onlyMessageIn<network::WorldState>(datagram).tick);
  }
  return ticks;
}

std::vector<std::uint32_t> ticksThatSurvive(
    const std::vector<std::uint32_t>& ticks) {
  // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp)
  std::mt19937 draws{LOSS_SEED};
  std::vector<std::uint32_t> survivors;
  for (const std::uint32_t tick : ticks) {
    if (draws() % ONE_LOST_IN != 0) {
      survivors.push_back(tick);
    }
  }
  return survivors;
}

std::uint32_t longestGapInTicks(const std::vector<std::uint32_t>& ticks,
                                std::uint32_t lastTick) {
  std::uint32_t longest = 0;
  std::uint32_t previous = 0;
  for (const std::uint32_t tick : ticks) {
    longest = std::max(longest, tick - previous);
    previous = tick;
  }
  return std::max(longest, lastTick - previous);
}

}  // namespace rtype::server::testing
