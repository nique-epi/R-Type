#include "SeatIndex.hpp"
#include <cstddef>
#include "ConnectedPlayer.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

std::size_t seatIndexOf(const ConnectedPlayer& player) {
  return static_cast<std::size_t>(player.playerNumber -
                                  network::FIRST_PLAYER_NUMBER);
}

}  // namespace rtype::server
