#include "Bounties.hpp"
#include <algorithm>
#include <cstdint>
#include <limits>
#include "ConnectedPlayer.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

std::uint32_t saturatedCount(std::uint64_t amount) {
  return static_cast<std::uint32_t>(std::min<std::uint64_t>(
      amount, std::numeric_limits<std::uint32_t>::max()));
}

void payEveryPlayer(PlayerRoster& roster, std::uint32_t kopecks) {
  for (ConnectedPlayer* const player : roster.connected()) {
    player->bounty = saturatedCount(std::uint64_t{player->bounty} + kopecks);
  }
}

}  // namespace rtype::server
