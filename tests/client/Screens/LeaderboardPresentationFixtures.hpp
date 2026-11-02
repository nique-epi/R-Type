#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "LeaderboardAnswer.hpp"
#include "LeaderboardPage.hpp"

/** @returns An entry of a leaderboard, played on 18 October 2026. */
[[nodiscard]] rtype::network::LeaderboardRow entryOf(std::uint32_t rank,
                                                     const std::string& name);

/** @returns A page answered with these entries and this entry of the player. */
[[nodiscard]] rtype::client::LeaderboardPage answeredPage(
    std::vector<rtype::network::LeaderboardRow> rows,
    std::optional<rtype::network::LeaderboardRow> own);
