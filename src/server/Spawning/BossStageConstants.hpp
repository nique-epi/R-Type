#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief Logger name the boss stage writes its log lines under. */
constexpr std::string_view BOSS_STAGE_LOGGER_NAME = "BossStage";

/** @brief How long the players are warned before the boss appears: 600 ms of
 * silence, the 3200 ms WARNING, and 600 ms of margin. */
constexpr engine::Duration BOSS_ARRIVAL_WARNING =
    std::chrono::milliseconds(4400);
constexpr auto BOSS_ARRIVAL_WARNING_MILLISECONDS = static_cast<std::uint32_t>(
    std::chrono::duration_cast<std::chrono::milliseconds>(BOSS_ARRIVAL_WARNING)
        .count());
constexpr std::size_t BOSS_ARRIVAL_WARNING_TICKS =
    engine::wholeTicksIn(BOSS_ARRIVAL_WARNING);

/** @brief Where the center of the boss appears: half its 512-unit frame past
 * the right edge, so it is wholly off screen, on the middle row of the area
 * of play. */
constexpr float BOSS_ENTRY_MARGIN = 256.0F;
constexpr game::Position BOSS_ENTRY_POSITION{
    .x = game::PLAYFIELD_WIDTH + BOSS_ENTRY_MARGIN,
    .y = game::PLAY_AREA_HEIGHT / 2.0F};

}  // namespace rtype::server
