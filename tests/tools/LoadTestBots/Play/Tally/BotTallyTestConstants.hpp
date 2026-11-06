#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "BotTraffic.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

/** @brief Three ticks in a row, as a game sends its world states. */
constexpr std::uint32_t FIRST_TICK = 5;
constexpr std::uint32_t SECOND_TICK = 6;
constexpr std::uint32_t THIRD_TICK = 7;

constexpr std::size_t TWO_WORLD_STATES = 2;
constexpr std::size_t THREE_WORLD_STATES = 3;

/** @brief A frame of the bots, and two of them in a row. */
constexpr engine::Duration A_FRAME = std::chrono::milliseconds{16};
constexpr engine::Duration TWO_FRAMES = std::chrono::milliseconds{32};

/** @brief What the session says of the game when the tally adds a frame. */
constexpr bool THE_GAME_RUNS = true;
constexpr bool THE_SESSION_ENDED_IT = false;

/** @brief The entities the world states of two ticks list, and both
 * together. */
constexpr std::size_t THREE_ENTITIES = 3;
constexpr std::size_t TWO_ENTITIES = 2;
constexpr std::size_t ENTITIES_OF_BOTH_TICKS = THREE_ENTITIES + TWO_ENTITIES;

/** @brief What the socket of a bot counted before the first world state, a
 * frame later, and what crossed it in between; the network carried fewer
 * bytes than the client read, since the server compressed them. */
constexpr BotTraffic TOTALS_BEFORE_THE_GAME{
    .plain = {.sent = {.count = 10, .bytes = 300},
              .received = {.count = 20, .bytes = 4000}},
    .wire = {.sent = {.count = 10, .bytes = 300},
             .received = {.count = 20, .bytes = 2500}}};
constexpr BotTraffic TOTALS_A_FRAME_LATER{
    .plain = {.sent = {.count = 11, .bytes = 330},
              .received = {.count = 22, .bytes = 4800}},
    .wire = {.sent = {.count = 11, .bytes = 330},
             .received = {.count = 22, .bytes = 3000}}};
constexpr BotTraffic TRAFFIC_OF_THAT_FRAME{
    .plain = {.sent = {.count = 1, .bytes = 30},
              .received = {.count = 2, .bytes = 800}},
    .wire = {.sent = {.count = 1, .bytes = 30},
             .received = {.count = 2, .bytes = 500}}};

/** @brief No traffic at all. */
constexpr BotTraffic NO_TRAFFIC{};

}  // namespace rtype::tools::testing
