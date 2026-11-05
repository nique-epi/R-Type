#include "PlayerCount.hpp"
#include <cstdint>
#include "HostedGame.hpp"
#include "ServerHeartbeat.hpp"

namespace rtype::master {

std::int64_t playerCountOf(const network::ServerHeartbeat& heartbeat) {
  std::int64_t count = 0;
  for (const network::HostedGame& game : heartbeat.games) {
    count += static_cast<std::int64_t>(game.players.size());
  }
  return count;
}

}  // namespace rtype::master
