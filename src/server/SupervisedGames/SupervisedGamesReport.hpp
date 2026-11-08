#pragma once

#include <span>
#include <vector>
#include "HostedGame.hpp"
#include "ServerMeasures.hpp"
#include "ServerOccupancy.hpp"
#include "SupervisedGame.hpp"

namespace rtype::server {

/**
 * @returns Each of @p games as the master lists it, by place: its number is
 * its place, its name GAME_NAME_PREFIX and the place, its difficulty, its
 * lives and its friendly fire those of its rules, its players the nicknames of
 * its newest status. A game
 * still starting is Waiting; a game that waits for players is Full once they
 * take every seat.
 */
[[nodiscard]] std::vector<network::HostedGame> supervisedHostedGames(
    std::span<const SupervisedGame> games);

/**
 * @returns The measures of @p games the master reads: the median and the 95th
 * percentile of the ticks of the slowest game, each the highest any game
 * reports, and the mean round trip of every player, each game counting as many
 * times as it has players. A measure no game reports stays empty.
 */
[[nodiscard]] network::ServerMeasures supervisedMeasures(
    std::span<const SupervisedGame> games);

/**
 * @returns How full the supervisor running @p games is: the players of its
 * games, MAXIMUM_RUNNING_GAMES games, and as free seats the empty seats of the
 * games that wait for players or start, plus network::MAXIMUM_PLAYERS for each
 * place no game holds. A game in progress or finished gives no seat.
 */
[[nodiscard]] ServerOccupancy supervisedOccupancy(
    std::span<const SupervisedGame> games);

}  // namespace rtype::server
