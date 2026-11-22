#include "Bounties.hpp"
#include <cstdint>
#include <limits>
#include "ConnectedPlayer.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

void payEveryPlayer(PlayerRoster& roster, std::uint32_t kopecks) {
  for (ConnectedPlayer* const player : roster.connected()) {
    const std::uint32_t room =
        std::numeric_limits<std::uint32_t>::max() - player->bounty;
    player->bounty += kopecks < room ? kopecks : room;
  }
}

}  // namespace rtype::server
