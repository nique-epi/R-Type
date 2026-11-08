#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "Ban.hpp"
#include "BanCommandsFixture.hpp"
#include "BanCommandsTestConstants.hpp"
#include "BanKind.hpp"
#include "BanTarget.hpp"
#include "BanTestConstants.hpp"
#include "BansDatabase.hpp"

using rtype::server::Ban;
using rtype::server::BanKind;
using rtype::server::BansDatabase;
using rtype::server::BanTarget;
using rtype::server::testing::BAN_MOMENT;
using rtype::server::testing::BANNED_ADDRESS;
using rtype::server::testing::BANNED_NICKNAME;

/**
 * Given the ban commands with no ban
 * When ACE is banned for two hours
 * Then the book holds that ban, from now to two hours later, and the console
 * says so
 */
TEST_F(BanCommandsFixture, BansANicknameForALength) {
  EXPECT_TRUE(run(BAN_ACE_FOR_TWO_HOURS_LINE).accepted);

  EXPECT_EQ(book().list().bans(),
            (std::vector{
                Ban{.target = BanTarget{.kind = BanKind::Nickname,
                                        .text = std::string{BANNED_NICKNAME}},
                    .bannedAt = BAN_MOMENT,
                    .expiresAt = BAN_MOMENT + TWO_HOURS_OF_BAN}}));
  EXPECT_EQ(lines(), std::vector{std::string{ACE_BANNED_FOR_TWO_HOURS}});
}

/**
 * Given the ban commands with no ban
 * When an address is banned with no length
 * Then the book holds a ban for good of that address, and the console says so
 */
TEST_F(BanCommandsFixture, BansAnAddressForGood) {
  EXPECT_TRUE(run(BAN_ADDRESS_FOR_GOOD_LINE).accepted);

  EXPECT_EQ(
      book().list().bans(),
      (std::vector{Ban{.target = BanTarget{.kind = BanKind::Address,
                                           .text = std::string{BANNED_ADDRESS}},
                       .bannedAt = BAN_MOMENT,
                       .expiresAt = std::nullopt}}));
  EXPECT_EQ(lines(), std::vector{std::string{ADDRESS_BANNED_FOR_GOOD}});
}

/**
 * Given a ban of ACE typed in the console
 * When another connection reads the file, as a game does
 * Then it finds that ban
 */
TEST_F(BanCommandsFixture, WritesTheBanToTheFileTheGamesRead) {
  ASSERT_TRUE(run(BAN_ACE_FOR_TWO_HOURS_LINE).accepted);

  EXPECT_EQ(BansDatabase{file()}.inForce(BAN_MOMENT).size(), 1U);
}

/**
 * Given a word too long for a nickname that is no address either
 * When it is banned
 * Then the line is refused and the book holds no ban
 */
TEST_F(BanCommandsFixture, RefusesWhatIsNeitherANicknameNorAnAddress) {
  EXPECT_FALSE(run(BAN_A_WORD_TOO_LONG_LINE).accepted);

  EXPECT_TRUE(book().list().bans().empty());
}

/**
 * Given a length of an unknown unit
 * When ACE is banned for it
 * Then the line is refused and the book holds no ban
 */
TEST_F(BanCommandsFixture, RefusesALengthItCannotRead) {
  EXPECT_FALSE(run(BAN_FOR_AN_UNKNOWN_LENGTH_LINE).accepted);

  EXPECT_TRUE(book().list().bans().empty());
}

/**
 * Given a ban line with three words, and one without a target
 * When each runs
 * Then both are refused and the book holds no ban
 */
TEST_F(BanCommandsFixture, RefusesALineWithoutOneOrTwoWords) {
  EXPECT_FALSE(run(BAN_WITH_THREE_WORDS_LINE).accepted);
  EXPECT_FALSE(run(BAN_WITHOUT_TARGET_LINE).accepted);

  EXPECT_TRUE(book().list().bans().empty());
}

/**
 * Given a ban of ACE
 * When ace, in small letters, is unbanned
 * Then the book and the file hold no ban, and the console says so
 */
TEST_F(BanCommandsFixture, UnbansAndSaysSo) {
  ASSERT_TRUE(run(BAN_ACE_FOR_TWO_HOURS_LINE).accepted);

  EXPECT_TRUE(run(UNBAN_ACE_IN_SMALL_LETTERS_LINE).accepted);

  EXPECT_TRUE(book().list().bans().empty());
  EXPECT_TRUE(BansDatabase{file()}.inForce(BAN_MOMENT).empty());
  EXPECT_EQ(lines().back(), ACE_UNBANNED);
}

/**
 * Given no ban
 * When ace is unbanned
 * Then the line is refused
 */
TEST_F(BanCommandsFixture, RefusesToUnbanWhatNoBanNames) {
  EXPECT_FALSE(run(UNBAN_ACE_IN_SMALL_LETTERS_LINE).accepted);
}

/**
 * Given a ban of two hours of ACE and a ban for good of an address, then a
 * minute and a half that passed
 * When the bans are listed
 * Then each has its line, ACE with its time left rounded up to the minute
 */
TEST_F(BanCommandsFixture, ListsTheBansInForceWithTheirTimeLeft) {
  ASSERT_TRUE(run(BAN_ACE_FOR_TWO_HOURS_LINE).accepted);
  ASSERT_TRUE(run(BAN_ADDRESS_FOR_GOOD_LINE).accepted);
  wait(A_MINUTE_AND_A_HALF);

  EXPECT_TRUE(run(BANS_LINE).accepted);

  const std::vector<std::string> written = lines();
  EXPECT_EQ(std::vector(written.end() - 2, written.end()),
            (std::vector{std::string{ACE_WITH_ONE_HOUR_FIFTY_NINE_LEFT},
                         std::string{ADDRESS_FOR_GOOD}}));
}

/**
 * Given a ban of thirty minutes of ACE, then thirty minutes that passed
 * When the bans are listed
 * Then the console says there is no ban
 */
TEST_F(BanCommandsFixture, ListsNoBanThatEnded) {
  ASSERT_TRUE(run(BAN_ACE_FOR_THIRTY_MINUTES_LINE).accepted);
  wait(THIRTY_MINUTES_OF_BAN);

  EXPECT_TRUE(run(BANS_LINE).accepted);

  EXPECT_EQ(lines().back(), NO_BAN_LINE);
}

/**
 * Given no ban
 * When the bans are listed
 * Then the console says there is no ban, in one line
 */
TEST_F(BanCommandsFixture, SaysNoBanWhenThereIsNone) {
  EXPECT_TRUE(run(BANS_LINE).accepted);

  EXPECT_EQ(lines(), std::vector{std::string{NO_BAN_LINE}});
}
