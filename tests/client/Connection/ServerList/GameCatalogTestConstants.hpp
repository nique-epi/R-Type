#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The two servers whose games are asked for. */
constexpr std::int64_t FIRST_SERVER_ID = 7;
constexpr std::int64_t SECOND_SERVER_ID = 8;
constexpr std::uint8_t PLAYERS_PER_GAME_OF_THE_SERVER = 4;

/** @brief The games of the listings, by number. */
constexpr std::uint16_t FIRST_GAME_NUMBER = 1;
constexpr std::uint16_t SECOND_GAME_NUMBER = 2;
constexpr std::string_view FIRST_GAME_NAME = "GAME 1";
constexpr std::string_view SECOND_GAME_NAME = "GAME 2";
constexpr std::uint8_t TWO_PILOTS = 2;

/** @brief The number of the first ask for games. */
constexpr std::uint64_t FIRST_GAME_ASK = 1;

}  // namespace rtype::client::testing
