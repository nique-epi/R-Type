#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <vector>
#include "GameInstance.hpp"
#include "GameState.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"

namespace rtype::server {

/**
 * @returns How the master calls a game in @p state with @p players connected:
 * a game that waits for players is Full with MAXIMUM_PLAYERS of them and
 * Waiting with fewer; the other states keep their name.
 */
[[nodiscard]] network::HostedGameState hostedStateOf(GameState state,
                                                     std::size_t players);

/**
 * @returns The games of @p games as the master lists them, in the order given:
 * game n (from FIRST_HOSTED_GAME_NUMBER) is named GAME_NAME_PREFIX and n, with
 * the nickname of each of its players in the order of their numbers, and its
 * difficulty. The caller keeps the games within MAXIMUM_HOSTED_GAMES, a bound
 * the games module checks when it starts.
 */
[[nodiscard]] std::vector<network::HostedGame> hostedGamesOf(
    std::span<const std::unique_ptr<GameInstance>> games);

}  // namespace rtype::server
