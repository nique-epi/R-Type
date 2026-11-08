#pragma once

#include <cstddef>
#include <string_view>
#include "SummoningConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief Logger name the volley system writes its log lines under. */
constexpr std::string_view VOLLEY_SYSTEM_LOGGER_NAME = "VolleySystem";

/** @brief The ticks an enemy of a volley stays summoning (see
 * game::Summoning) before it takes part: game::SUMMONING_TIME, as long as
 * the client shows the warning of its arrival. */
constexpr std::size_t SUMMONING_TICKS =
    engine::wholeTicksIn(game::SUMMONING_TIME);

/** @brief An enemy of a volley appears at least SUMMONED_ROW_MARGIN units
 * inside the top of the playfield and the status band, so its warning
 * shows whole. */
constexpr float SUMMONED_ROW_MARGIN = 60.0F;

}  // namespace rtype::server
