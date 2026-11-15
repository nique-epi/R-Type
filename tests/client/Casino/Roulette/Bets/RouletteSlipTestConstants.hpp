#pragma once

#include <cstddef>
#include <cstdint>
#include "RouletteBets.hpp"

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

/** @brief A chip of 500, ten of which make the 5 000 the server takes in a
 * round at most. */
constexpr network::RouletteBet FIVE_HUNDRED_ON_RED{
    .kind = network::RouletteBetKind::Red, .firstNumber = 0, .amount = 500};
constexpr std::size_t CHIPS_OF_500_IN_THE_CAP = 10;
constexpr std::uint32_t ROUND_CAP = 5'000;

/** @brief The stacks the server takes in a round at most, 32: a chip of 1 on
 * each number from 1 to 32; then one more on 33. */
constexpr std::uint8_t LAST_NUMBER_OF_THE_STACKS = 32;
constexpr std::size_t MOST_STACKS = 32;
constexpr network::RouletteBet ONE_ON_THIRTY_THREE{
    .kind = network::RouletteBetKind::Straight, .firstNumber = 33, .amount = 1};

}  // namespace rtype::client::testing
