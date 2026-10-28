#pragma once

#include <string>

namespace rtype::client {

/** @returns How a player is named by its number: PLAYER_NUMBER_PREFIX, then
 * the number, P3 for player 3. */
[[nodiscard]] std::string playerNumberLabel(int playerNumber);

}  // namespace rtype::client
