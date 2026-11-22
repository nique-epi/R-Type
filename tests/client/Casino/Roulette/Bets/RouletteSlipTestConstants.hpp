#pragma once

#include <cstddef>
#include <cstdint>
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"

namespace rtype::client::testing {

/** @brief A chip of 25 on 17, the same again, and a chip of 25 on red. */
constexpr network::RouletteBet TWENTY_FIVE_ON_SEVENTEEN{
    .kind = network::RouletteBetKind::Straight,
    .firstNumber = 17,
    .amount = 25};
constexpr network::RouletteBet FIFTY_ON_SEVENTEEN{
    .kind = network::RouletteBetKind::Straight,
    .firstNumber = 17,
    .amount = 50};
constexpr network::RouletteBet TWENTY_FIVE_ON_RED{
    .kind = network::RouletteBetKind::Red, .firstNumber = 0, .amount = 25};

/** @brief Chips held in plenty, and chips held for one chip of 25 only. */
constexpr std::uint32_t PLENTY_OF_CHIPS = 100'000;
constexpr std::uint32_t CHIPS_FOR_ONE_CHIP = 40;

/** @brief A chip of 500, four of which make the 2 000 the server takes in a
 * round at most. */
constexpr network::RouletteBet FIVE_HUNDRED_ON_RED{
    .kind = network::RouletteBetKind::Red, .firstNumber = 0, .amount = 500};
constexpr std::size_t CHIPS_OF_500_IN_THE_CAP = 4;
constexpr std::uint32_t ROUND_CAP = 2'000;

/** @brief A chip of 500 on 17, which returns 36 times 500, the 18 000 a
 * round returns at most; a chip of 1 more on 17, which would make it 18 036;
 * and a chip of 500 on 18, a number the chip on 17 does not cover. */
constexpr network::RouletteBet FIVE_HUNDRED_ON_SEVENTEEN{
    .kind = network::RouletteBetKind::Straight,
    .firstNumber = 17,
    .amount = 500};
constexpr network::RouletteBet ONE_ON_SEVENTEEN{
    .kind = network::RouletteBetKind::Straight, .firstNumber = 17, .amount = 1};
constexpr network::RouletteBet FIVE_HUNDRED_ON_EIGHTEEN{
    .kind = network::RouletteBetKind::Straight,
    .firstNumber = 18,
    .amount = 500};
constexpr std::uint32_t ONE_CHIP_OF_500 = 500;
constexpr std::uint32_t TWO_CHIPS_OF_500 = 1'000;

/** @brief The stacks the server takes in a round at most, 32: a chip of 1 on
 * each number from 1 to 32; then one more on 33. */
constexpr std::uint8_t LAST_NUMBER_OF_THE_STACKS = 32;
constexpr std::size_t MOST_STACKS = 32;
constexpr network::RouletteBet ONE_ON_THIRTY_THREE{
    .kind = network::RouletteBetKind::Straight, .firstNumber = 33, .amount = 1};

}  // namespace rtype::client::testing
