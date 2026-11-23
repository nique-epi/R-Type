#pragma once

#include <cstddef>
#include "TeamFirepower.hpp"
#include "ThreatLevel.hpp"

namespace rtype::server::testing {

/** @brief A team that fires its strongest weapon, five times its gun, for the
 * 8 s the threat takes to rise the whole way. */
constexpr int STRONGEST_WEAPON_PERCENT = 500;
constexpr game::TeamFirepower TEAM_WITH_THE_STRONGEST{
    .meanPercent = STRONGEST_WEAPON_PERCENT,
    .highestPercent = STRONGEST_WEAPON_PERCENT};
constexpr std::size_t WHOLE_RISE_TICKS = 480;

constexpr game::ThreatLevel NO_THREAT{};
constexpr game::ThreatLevel THE_MOST_THREAT{.percent = 100, .tier = 3};

}  // namespace rtype::server::testing
