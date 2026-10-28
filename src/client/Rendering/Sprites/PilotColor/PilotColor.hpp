#pragma once

namespace rtype::client {

/** @returns Whether a color is one of the colors of ship, 0 to
 * PLAYER_COLOR_COUNT - 1. */
[[nodiscard]] bool isPilotColor(int color);

/** @returns The pilot color of the player numbered @p playerNumber: its
 * number less FIRST_PLAYER_NUMBER, the row of its ship in the sheets. */
[[nodiscard]] int pilotColorOfPlayerNumber(int playerNumber);

/** @returns The number of the player who flies @p pilotColor: the color plus
 * FIRST_PLAYER_NUMBER, the reverse of pilotColorOfPlayerNumber(). */
[[nodiscard]] int playerNumberOfPilotColor(int pilotColor);

}  // namespace rtype::client
