#include <gtest/gtest.h>
#include <algorithm>
#include <string>
#include "CasinoBar.hpp"
#include "CasinoBarChipTestConstants.hpp"
#include "MenuBuilder.hpp"

using rtype::client::CasinoBar;
using rtype::client::MenuBuilder;
using rtype::client::testing::LABELLED_CASINO_BARS;

/**
 * Given every labelled bar of the casino, each with an area of a fixed width
 * When its French label and its keycap are laid out as a chip
 * Then they need no more room than the bar gives, or than its English label
 */
TEST(CasinoBarChip, EveryFrenchLabelFitsItsBar) {
  for (const CasinoBar& bar : LABELLED_CASINO_BARS) {
    SCOPED_TRACE(std::string(bar.label.english));
    const int french =
        MenuBuilder::chipArea(
            {.label = std::string(bar.label.french), .keyName = bar.keyName})
            .size.x;
    const int english =
        MenuBuilder::chipArea(
            {.label = std::string(bar.label.english), .keyName = bar.keyName})
            .size.x;
    EXPECT_LE(french, std::max(bar.area.size.x, english));
  }
}
