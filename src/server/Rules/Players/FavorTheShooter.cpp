#include "FavorTheShooter.hpp"
#include <cstdint>
#include "HitArbitrationConstants.hpp"
#include "PlayerRoster.hpp"
#include "ShooterViews.hpp"
#include "World.hpp"

namespace rtype::server {

void FavorTheShooter::giveViews(game::World& world, PlayerRoster& roster,
                                std::uint32_t tick) const {
  giveShooterViews(world, roster, tick);
  capViewDelays(world, SHOOTER_REWIND_TICKS);
}

}  // namespace rtype::server
