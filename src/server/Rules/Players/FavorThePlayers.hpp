#pragma once

#include <cstdint>
#include "IHitArbitration.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief The arbitration of a game of players against the Bydos: a missile of
 * a player is tested where its player saw its target, and a ship where its
 * player saw the enemies and their missiles, so a player hits what it saw hit
 * and dodges what it saw miss. In co-op the shooter facing a player is a
 * Bydo, which loses nothing by it.
 */
class FavorThePlayers final : public IHitArbitration {
 public:
  void giveViews(game::World& world, PlayerRoster& roster,
                 std::uint32_t tick) const override;
};

}  // namespace rtype::server
