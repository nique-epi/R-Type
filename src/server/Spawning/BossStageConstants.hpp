#pragma once

#include <string_view>
#include "PlayfieldConstants.hpp"
#include "Position.hpp"

namespace rtype::server {

/** @brief Logger name the boss stage writes its log lines under. */
constexpr std::string_view BOSS_STAGE_LOGGER_NAME = "BossStage";

/** @brief Where the center of the boss of the waves appears: half its
 * 512-unit frame past the right edge, so it is wholly off screen, on the
 * middle row of the area of play. */
constexpr float BOSS_ENTRY_MARGIN = 256.0F;
constexpr game::Position BOSS_ENTRY_POSITION{
    .x = game::PLAYFIELD_WIDTH + BOSS_ENTRY_MARGIN,
    .y = game::PLAY_AREA_HEIGHT / 2.0F};

}  // namespace rtype::server
