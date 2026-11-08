#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include "BanKind.hpp"
#include "BanTarget.hpp"
#include "BanWords.hpp"
#include "BanWordsTestConstants.hpp"

using rtype::server::BanKind;
using rtype::server::banLengthOf;
using rtype::server::BanTarget;
using rtype::server::banTargetOf;

/**
 * Given a word that reads as an IPv4 address
 * When the target it names is read
 * Then it is that address
 */
TEST(BanWords, ReadsAnIpv4AddressAsAnAddress) {
  EXPECT_EQ(banTargetOf(WORD_OF_AN_ADDRESS),
            (BanTarget{.kind = BanKind::Address,
                       .text = std::string{WORD_OF_AN_ADDRESS}}));
}

/**
 * Given words that look like addresses but are none: a number that starts
 * with 0, and a number past 255
 * When the target each names is read
 * Then each is a nickname, as the protocol accepts such a nickname
 */
TEST(BanWords, ReadsWhatIsNoAddressAsANickname) {
  for (const std::string_view word :
       {ADDRESS_WITH_A_LEADING_ZERO, ADDRESS_PAST_255}) {
    SCOPED_TRACE(word);
    EXPECT_EQ(banTargetOf(word), (BanTarget{.kind = BanKind::Nickname,
                                            .text = std::string{word}}));
  }
}

/**
 * Given a word of 17 bytes, one more than a nickname holds
 * When the target it names is read
 * Then there is none
 */
TEST(BanWords, RefusesAWordThatIsNeitherANicknameNorAnAddress) {
  EXPECT_EQ(banTargetOf(SEVENTEEN_BYTE_WORD), std::nullopt);
}

/**
 * Given a length in minutes, in hours, in days, and the largest one
 * When each is read
 * Then each gives its seconds
 */
TEST(BanWords, ReadsTheLengthsOfTheThreeUnits) {
  EXPECT_EQ(banLengthOf(THIRTY_MINUTES_WORD), THIRTY_MINUTES);
  EXPECT_EQ(banLengthOf(TWO_HOURS_WORD), TWO_HOURS);
  EXPECT_EQ(banLengthOf(SEVEN_DAYS_WORD), SEVEN_DAYS);
  EXPECT_EQ(banLengthOf(LARGEST_LENGTH_WORD), LARGEST_LENGTH);
}

/**
 * Given a count of zero, and a count one past the largest
 * When each is read as a length
 * Then neither is one
 */
TEST(BanWords, RefusesACountOutsideTheRange) {
  EXPECT_EQ(banLengthOf(ZERO_LENGTH_WORD), std::nullopt);
  EXPECT_EQ(banLengthOf(LENGTH_PAST_THE_LARGEST_WORD), std::nullopt);
}

/**
 * Given words without a unit, without a count, with an unknown unit, with a
 * sign, with a space, and an empty word
 * When each is read as a length
 * Then none is one
 */
TEST(BanWords, RefusesAWordThatIsNoLength) {
  for (const std::string_view word :
       {COUNT_WITHOUT_UNIT_WORD, UNIT_WITHOUT_COUNT_WORD, UNKNOWN_UNIT_WORD,
        NEGATIVE_LENGTH_WORD, SIGNED_LENGTH_WORD, LENGTH_WITH_A_SPACE_WORD,
        std::string_view{}}) {
    SCOPED_TRACE(word);
    EXPECT_EQ(banLengthOf(word), std::nullopt);
  }
}
