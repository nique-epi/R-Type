#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief How the master is to see the games of a server of three games. */
constexpr std::string_view FIRST_HOSTED_NAME = "GAME 1";
constexpr std::string_view SECOND_HOSTED_NAME = "GAME 2";
constexpr std::string_view THIRD_HOSTED_NAME = "GAME 3";
constexpr std::string_view HOSTED_DIFFICULTY = "normal";

/** @brief The nicknames of the players the tests seat, in seat order. */
constexpr std::string_view FIRST_HOSTED_PILOT = "ACE";
constexpr std::string_view SECOND_HOSTED_PILOT = "BYDO_HUNTER";
constexpr std::string_view THIRD_HOSTED_PILOT = "R9";

/** @brief The players of a game that waits, one short of every seat. */
constexpr std::size_t THREE_PLAYERS = 3;
constexpr std::size_t FOUR_PLAYERS = 4;
constexpr std::size_t NO_PLAYER = 0;
