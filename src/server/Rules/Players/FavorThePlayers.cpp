#include "FavorThePlayers.hpp"
#include <cstdint>
#include "HitArbitrationConstants.hpp"
#include "PlayerRoster.hpp"
#include "ShooterViews.hpp"
#include "World.hpp"

namespace rtype::server {

void FavorThePlayers::giveViews(game::World& world, PlayerRoster& roster,
                                std::uint32_t tick) const {
  giveShooterViews(world, roster, tick);
  giveShipViews(world, roster, tick);
  capViewDelays(world, PLAYERS_REWIND_TICKS);
}

}  // namespace rtype::server
