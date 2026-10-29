#pragma once

#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "Intent.hpp"
#include "PlayerInput.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @returns Whether @p input holds the action at @p bitPosition of its actions
 * byte.
 */
[[nodiscard]] bool holdsActionAt(const network::PlayerInput& input,
                                 std::uint8_t bitPosition);

/**
 * @returns The directions @p input holds for the ship; shooting is not one.
 */
[[nodiscard]] game::ShipDirections directionsOf(
    const network::PlayerInput& input);

/**
 * @brief Gives the ship of @p player in @p world the presses of @p input,
 * which may start a roll or a dash (game::motionAfterInput()); nothing when
 * the player has no ship in that world. Runs before @p input becomes the
 * Intent of the ship, which holds the directions it compares to. A ship no
 * input reached yet knows no directions: when its player sent inputs to an
 * earlier ship, its first input presses nothing, so a key held through a
 * respawn starts no maneuver. GameInstance gives it to its sessions as their
 * onNewerInput.
 */
void noteTaps(game::World& world, const ConnectedPlayer& player,
              const network::PlayerInput& input);

}  // namespace rtype::server
