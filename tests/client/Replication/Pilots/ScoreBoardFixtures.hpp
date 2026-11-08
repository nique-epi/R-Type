#pragma once

#include <cstdint>
#include "LobbyState.hpp"
#include "ScoreBoard.hpp"
#include "WorldState.hpp"

/** @returns A player of the waiting room with this nickname, not ready. */
[[nodiscard]] rtype::network::LobbyPlayer waiting(std::uint8_t number,
                                                  const char* nickname);

/** @returns A player listed by a world state with these lives, its ship
 * flying, and SCORE. */
[[nodiscard]] rtype::network::PlayerState listedPlayer(std::uint8_t number,
                                                       std::uint8_t lives);

/** @returns A board started from the waiting room of players 3, 1 and 2,
 * this client being player 2. */
[[nodiscard]] rtype::client::ScoreBoard scoreBoardOfThree();
