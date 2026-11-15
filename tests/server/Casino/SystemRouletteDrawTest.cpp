#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include "ProtocolConstants.hpp"
#include "SystemRouletteDraw.hpp"
#include "SystemRouletteDrawTestConstants.hpp"

namespace rtype::server::testing {

/**
 * Given the draw of the system
 * When it draws 37 000 numbers
 * Then none is past the wheel, and every number of the wheel came out
 */
TEST(SystemRouletteDraw, DrawsEveryNumberOfTheWheelAndNoOther) {
  SystemRouletteDraw draw;
  std::array<bool, network::HIGHEST_ROULETTE_NUMBER + 1> seen{};

  for (std::size_t index = 0; index < DRAWS_FOR_EVERY_NUMBER; ++index) {
    const std::uint8_t number = draw.drawNumber();
    ASSERT_LE(number, network::HIGHEST_ROULETTE_NUMBER);
    seen.at(number) = true;
  }

  for (std::size_t number = 0; number < seen.size(); ++number) {
    EXPECT_TRUE(seen.at(number)) << number << " never came out";
  }
}

}  // namespace rtype::server::testing
