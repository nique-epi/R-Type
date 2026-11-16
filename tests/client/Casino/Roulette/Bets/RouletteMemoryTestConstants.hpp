#pragma once

#include <array>
#include <cstdint>
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"

namespace rtype::client::testing {

/** @brief Two rounds of the table, one after the other. */
constexpr std::uint16_t FIRST_ROUND = 41;
constexpr std::uint16_t NEXT_ROUND = 42;

/** @brief The chips of the player, a chip of 25 on 17, and what the player
 * holds once it lies on the cloth. */
constexpr std::uint32_t CHIPS_OF_THE_PLAYER = 1'250;
constexpr network::RouletteBet A_CHIP_ON_SEVENTEEN{
    .kind = network::RouletteBetKind::Straight,
    .firstNumber = 17,
    .amount = 25};
constexpr std::uint32_t CHIPS_ONCE_LAID = 1'225;

/** @brief Eight numbers drawn, one round after another from FIRST_ROUND; the
 * table keeps the last seven, the newest first. */
constexpr std::array<std::uint8_t, 8> NUMBERS_DRAWN{4, 11, 26, 21,
                                                    8, 32, 0,  17};
constexpr std::array<std::uint8_t, 7> LAST_SEVEN_NEWEST_FIRST{17, 0,  32, 8,
                                                              21, 26, 11};
constexpr std::uint8_t SEVENTEEN_DRAWN = 17;

/** @brief How many copies of a result the server sends, every 500 ms. */
constexpr int COPIES_OF_A_RESULT = 3;

}  // namespace rtype::client::testing
