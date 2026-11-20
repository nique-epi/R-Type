#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief How fast a level scrolls while nothing pauses it, in logical units
 * per second: the speed of the nearest stars of the sky of the client. */
constexpr float LEVEL_SCROLL_SPEED = 120.0F;

/**
 * @brief The parts of a transition between two parts of a level, counted back
 * from its end, so a client that joins during one knows where it stands from
 * the time left alone: the ships fly out to the right for
 * LEVEL_TRANSITION_FLY_OUT, stay away for LEVEL_TRANSITION_AWAY, then come
 * back from the left for the last LEVEL_TRANSITION_FLY_IN, the server putting
 * them back at their start as it begins.
 */
constexpr engine::Duration LEVEL_TRANSITION_FLY_OUT =
    std::chrono::milliseconds(900);
constexpr engine::Duration LEVEL_TRANSITION_AWAY =
    std::chrono::milliseconds(1100);
constexpr engine::Duration LEVEL_TRANSITION_FLY_IN =
    std::chrono::milliseconds(1000);

/** @brief The ticks left of a transition as its ships come back: those of
 * LEVEL_TRANSITION_FLY_IN, 60. */
constexpr std::uint32_t LEVEL_TRANSITION_RETURN_TICKS =
    static_cast<std::uint32_t>(engine::wholeTicksIn(LEVEL_TRANSITION_FLY_IN));

/** @brief The shortest transition, its three parts end to end (3 s), and the
 * longest one, which a LevelStatus still carries in milliseconds on 16 bits. */
constexpr engine::Duration SHORTEST_LEVEL_TRANSITION =
    LEVEL_TRANSITION_FLY_OUT + LEVEL_TRANSITION_AWAY + LEVEL_TRANSITION_FLY_IN;
constexpr engine::Duration LONGEST_LEVEL_TRANSITION =
    std::chrono::milliseconds(60000);

}  // namespace rtype::game
