#pragma once

#include <cstdint>
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"

namespace rtype::client::testing {

/** @brief The room of the tests, and two rounds of its wheel, one after the
 * other. */
constexpr std::uint8_t MEMORY_ROOM = 1;
constexpr std::uint64_t FIRST_ROUND = 41;
constexpr std::uint64_t NEXT_ROUND = 42;

/** @brief The chips of the player, and those the wallet holds once the master
 * took its bets. */
constexpr std::uint32_t CHIPS_OF_THE_PLAYER = 1'250;
constexpr std::uint32_t CHIPS_LEFT = 1'225;

/** @brief A chip of 25 on 17, and one of 5 on red. */
constexpr network::RouletteBet A_CHIP_ON_SEVENTEEN{
    .kind = network::RouletteBetKind::Straight,
    .firstNumber = 17,
    .amount = 25};
constexpr network::RouletteBet A_CHIP_ON_RED{
    .kind = network::RouletteBetKind::Red, .firstNumber = 0, .amount = 5};

/** @brief The numbers of two answers of the master, one after the other. */
constexpr std::uint64_t FIRST_ANSWER = 5;
constexpr std::uint64_t NEXT_ANSWER = 6;

}  // namespace rtype::client::testing
