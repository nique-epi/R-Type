#pragma once

#include <optional>
#include <vector>
#include "ItemClaim.hpp"
#include "ItemTouch.hpp"

namespace rtype::game {

class World;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/** @returns The claim of the pilot of the ship of @p touch on its item, when
 * both carry a network id and the ship belongs to a player of @p roster;
 * nothing otherwise. */
[[nodiscard]] std::optional<ItemClaim> itemClaimOf(game::World& world,
                                                   PlayerRoster& roster,
                                                   ItemTouch touch);

/** @returns For each item of @p claims, the claim that wins it: the lowest
 * player number among the pilots who touched it and still fly the ship that
 * touched it, so a ship destroyed in the same tick wins nothing. An item no
 * such pilot claims has no winner. In the order of the network ids of the
 * items. */
[[nodiscard]] std::vector<ItemClaim> winningClaims(
    std::vector<ItemClaim> claims, PlayerRoster& roster);

}  // namespace rtype::server
