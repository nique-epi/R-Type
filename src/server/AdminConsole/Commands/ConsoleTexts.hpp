#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ServerMeasures.hpp"
#include "SupervisedGame.hpp"
#include "TrafficTotals.hpp"

namespace rtype::server {

/**
 * @returns One line per game of @p games, in the order they started: its
 * place, number, state, pilots and port, then the 95th percentile of its tick
 * and its mean round trip when its status gave them; "no game runs" when
 * there is none.
 */
[[nodiscard]] std::vector<std::string> gameLinesOf(
    std::span<const SupervisedGame> games);

/** @returns One line per game: its place and the nicknames of its pilots, or
 * "no pilot"; "no game runs" when there is none. */
[[nodiscard]] std::vector<std::string> playerLinesOf(
    std::span<const SupervisedGame> games);

/** @returns The line close-game writes once it sent the stop order of
 * @p game. */
[[nodiscard]] std::string closingLineOf(const SupervisedGame& game);

/** @returns The line kick writes once it sent @p game the order to expel
 * @p nickname. */
[[nodiscard]] std::string kickLineOf(const SupervisedGame& game,
                                     std::string_view nickname);

/**
 * @returns What crossed the socket of the supervisor over @p elapsed (see
 * network::trafficLineOf()), then the datagrams rejected since the start.
 */
[[nodiscard]] std::vector<std::string> networkLinesOf(
    const network::TrafficTotals& traffic, std::chrono::nanoseconds elapsed,
    std::size_t rejected);

/** @returns The line of the measures of the games: each measure of
 * @p measures that was taken, or "games: no measure yet" when none was, as
 * when no game ran yet. */
[[nodiscard]] std::string measuresLineOf(
    const std::optional<network::ServerMeasures>& measures);

}  // namespace rtype::server
