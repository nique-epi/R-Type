#pragma once

#include "IInterfaceCanvas.hpp"
#include "RouletteView.hpp"

namespace rtype::client {

/**
 * @brief Paints @p view, everything the roulette table shows but its bars:
 * the room, the croupier behind the table, the table and its cloth, the
 * wheel, its bowl, its rotor and its ball, the chips of the player, the
 * cursor, the number drawn lit with its plaque, the numbers drawn before,
 * the pilots seated, then the band.
 */
void paintRoulette(IInterfaceCanvas& canvas, const RouletteView& view);

}  // namespace rtype::client
