#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief Where the supervisor is, in the tests, and the token it shares with
 * the game process. */
constexpr std::uint16_t SUPERVISOR_PORT = 4242;
constexpr std::uint64_t LINK_TOKEN = 0xFEED;

/** @brief How long the fixture waits to see a stop requested. */
constexpr std::chrono::milliseconds STOP_OBSERVATION_WAIT{50};

/** @brief The delays of the requirement: just under a second, a second, five
 * seconds and one tick more, and the supervisor beats of five seconds. */
constexpr rtype::engine::Duration JUST_UNDER_A_SECOND =
    std::chrono::milliseconds(980);
constexpr rtype::engine::Duration ONE_SECOND = std::chrono::seconds(1);
constexpr rtype::engine::Duration FIVE_SECONDS_AND_A_TICK =
    std::chrono::seconds(5) + rtype::engine::SIMULATION_TICK_DURATION;
constexpr int FIVE_SECONDS_OF_BEATS = 5;

/** @brief The pilot the supervisor expels, the nickname every seated player
 * of the tests has (PLAYER_NICKNAME, "ace") written in capitals, since
 * nicknames are compared without the case of ASCII letters, and a nickname no
 * player has. */
constexpr std::size_t EXPELLED_PILOT_INDEX = 0;
constexpr std::string_view SEATED_NICKNAME_IN_CAPITALS = "ACE";
constexpr std::string_view NICKNAME_NOBODY_HAS = "NOVA";
