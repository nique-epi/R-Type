#include "OccupancyCount.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "Games.hpp"
#include "ProtocolConstants.hpp"
#include "ServerOccupancy.hpp"

namespace rtype::server {

ServerOccupancy occupancyOf(const Games& games) {
  ServerOccupancy occupancy{
      .playerCount = 0,
      .gameCount = static_cast<std::uint16_t>(games.all().size()),
      .freeSeatCount = 0};
  for (const std::unique_ptr<GameInstance>& game : games.all()) {
    const std::size_t seated = game->roster().connected().size();
    occupancy.playerCount =
        static_cast<std::uint16_t>(occupancy.playerCount + seated);
    if (game->state() == GameState::WaitingForPlayers) {
      occupancy.freeSeatCount = static_cast<std::uint16_t>(
          occupancy.freeSeatCount + network::MAXIMUM_PLAYERS - seated);
    }
  }
  return occupancy;
}

}  // namespace rtype::server
