#include "FavorThePlayers.hpp"
#include <cstdint>
#include "PlayerRoster.hpp"
#include "ShooterViews.hpp"
#include "World.hpp"

namespace rtype::server {

void FavorThePlayers::giveViews(game::World& world, PlayerRoster& roster,
                                std::uint32_t tick) const {
  giveShooterViews(world, roster, tick);
  giveShipViews(world, roster, tick);
}

}  // namespace rtype::server
