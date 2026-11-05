#pragma once

#include <string_view>
#include <vector>
#include "GameState.hpp"
#include "Games.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"

namespace rtype::server {

/** @returns How the master names @p state. */
[[nodiscard]] network::HostedGameState hostedStateOf(GameState state);

/**
 * @returns Each game of @p games as the master lists it: its number, from
 * network::FIRST_HOSTED_GAME_NUMBER, its name (GAME_NAME_PREFIX and the
 * number), its state, @p difficulty and the nicknames of its connected
 * players. A game that waits for players is Full when they take every seat.
 */
[[nodiscard]] std::vector<network::HostedGame> hostedGamesOf(
    const Games& games, std::string_view difficulty);

}  // namespace rtype::server
