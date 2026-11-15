#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <ostream>
#include "RouletteBets.hpp"

namespace rtype::server::testing {

/** @brief A bet, a number drawn, and what the bet must return then. */
struct PayoutCase {
  network::RouletteBet bet;
  std::uint8_t drawn{};
  std::uint64_t payout{};
};

/** @brief The payout of a bet on a number, one case at a time. */
class RouletteRulesPayout : public ::testing::TestWithParam<PayoutCase> {};

/** @brief A bet that does not lie on the cloth, one at a time. */
class RouletteRulesRefused
    : public ::testing::TestWithParam<network::RouletteBet> {};

/** @brief A bet that lies on the cloth, one at a time. */
class RouletteRulesPlaced
    : public ::testing::TestWithParam<network::RouletteBet> {};

/** @brief Writes @p payoutCase in the name of a failing case. */
void PrintTo(const PayoutCase& payoutCase, std::ostream* output);

}  // namespace rtype::server::testing
