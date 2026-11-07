#include <gtest/gtest.h>
#include <string>
#include "EngineException.hpp"
#include "WholeNumber.hpp"
#include "WholeNumberTestConstants.hpp"

using rtype::engine::CommandRefusedException;
using rtype::engine::wholeNumberFrom;

/**
 * Given the text of a number
 * When it is read
 * Then it is that number
 */
TEST(WholeNumber, ReadsTheDigits) {
  EXPECT_EQ(wholeNumberFrom(std::string{FORTY_TWO_TEXT}), FORTY_TWO);
}

/**
 * Given a number followed by a letter
 * When it is read
 * Then the line is refused
 */
TEST(WholeNumber, RefusesWhatIsNotOnlyDigits) {
  EXPECT_THROW(static_cast<void>(
                   wholeNumberFrom(std::string{NUMBER_WITH_A_LETTER_TEXT})),
               CommandRefusedException);
}

/**
 * Given an empty text
 * When it is read
 * Then the line is refused
 */
TEST(WholeNumber, RefusesAnEmptyText) {
  EXPECT_THROW(static_cast<void>(wholeNumberFrom(std::string{})),
               CommandRefusedException);
}
