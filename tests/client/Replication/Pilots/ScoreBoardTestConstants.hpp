#pragma once

#include <cstddef>
#include <cstdint>

/** @brief The players of the board tests, by number, one number on each side
 * of the bounds, and this client, player 2. */
constexpr std::uint8_t PLAYER_ONE = 1;
constexpr std::uint8_t PLAYER_TWO = 2;
constexpr std::uint8_t PLAYER_THREE = 3;
constexpr std::uint8_t PLAYER_FOUR = 4;
constexpr std::uint8_t NO_PLAYER = 0;
constexpr std::uint8_t PAST_THE_LAST_PLAYER = 5;
constexpr std::uint8_t LOCAL_NUMBER = PLAYER_TWO;

/** @brief The score of a listed player, and the lives the tests give. */
constexpr std::uint32_t SCORE = 12400;
constexpr std::uint8_t LIVES = 3;
constexpr std::uint8_t LAST_LIFE = 1;
constexpr std::uint8_t NO_LIFE = 0;

/** @brief The pilots of a board of four. */
constexpr std::size_t PILOTS_OF_FOUR = 4;
