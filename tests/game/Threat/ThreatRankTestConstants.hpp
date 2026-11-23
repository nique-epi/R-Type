#pragma once

#include <cstddef>
#include "TeamFirepower.hpp"
#include "ThreatLevel.hpp"

namespace rtype::game::testing {

/** @brief The strongest weapon of the ships of the tests, five times their
 * gun, and teams of those ships. */
constexpr int STRONGEST_WEAPON_PERCENT = 500;
constexpr TeamFirepower TEAM_WITH_ITS_GUNS{
    .meanPercent = 100, .highestPercent = STRONGEST_WEAPON_PERCENT};
constexpr TeamFirepower TEAM_WITH_THE_STRONGEST{
    .meanPercent = STRONGEST_WEAPON_PERCENT,
    .highestPercent = STRONGEST_WEAPON_PERCENT};
constexpr TeamFirepower TEAM_PAST_THE_STRONGEST{
    .meanPercent = 900, .highestPercent = STRONGEST_WEAPON_PERCENT};

/** @brief A team that fires three times its gun with sheets that hold no
 * weapon: nothing to rise toward. */
constexpr TeamFirepower TEAM_WITHOUT_WEAPONS{.meanPercent = 300,
                                             .highestPercent = 100};

/** @brief A team a quarter of the way from its guns to the strongest: 200 %
 * of 100 to 500, so 120 of the 480 ticks; and one just under, 196 %, so 115
 * ticks, which read 23 %. */
constexpr TeamFirepower TEAM_A_QUARTER_OF_THE_WAY{
    .meanPercent = 200, .highestPercent = STRONGEST_WEAPON_PERCENT};
constexpr TeamFirepower TEAM_JUST_UNDER_A_QUARTER{
    .meanPercent = 196, .highestPercent = STRONGEST_WEAPON_PERCENT};

/** @brief The threat takes 8 s to rise the whole way at 60 ticks a second,
 * and half of it to rise half way; past it, it stays at the most. */
constexpr std::size_t WHOLE_RISE_TICKS = 480;
constexpr std::size_t HALF_RISE_TICKS = 240;
constexpr std::size_t LONGER_THAN_THE_RISE = 600;
constexpr std::size_t ONE_TICK = 1;

constexpr ThreatLevel NO_THREAT{};
constexpr ThreatLevel HALF_THREAT{.percent = 50, .tier = 2};
constexpr ThreatLevel ALMOST_THE_MOST{.percent = 99, .tier = 3};
constexpr ThreatLevel THE_MOST{.percent = 100, .tier = 3};
constexpr ThreatLevel A_QUARTER{.percent = 25, .tier = 1};
constexpr ThreatLevel JUST_UNDER_A_QUARTER{.percent = 23, .tier = 0};

}  // namespace rtype::game::testing
