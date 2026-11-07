#pragma once

#include <cstdint>
#include "IHitArbitration.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief The arbitration of a game between players: a missile of a player is
 * tested where its player saw its target, at most SHOOTER_REWIND before the
 * present, and a ship in the present of the server, as Overwatch and the
 * Source engine favor the shooter. A player with
 * a ping may then be hit by what it saw miss.
 */
class FavorTheShooter final : public IHitArbitration {
 public:
  void giveViews(game::World& world, PlayerRoster& roster,
                 std::uint32_t tick) const override;
};

}  // namespace rtype::server
