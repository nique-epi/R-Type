#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "Ban.hpp"
#include "BanCandidate.hpp"
#include "BanKind.hpp"
#include "BanList.hpp"
#include "BanTarget.hpp"
#include "BanTestConstants.hpp"

using rtype::server::Ban;
using rtype::server::BanCandidate;
using rtype::server::BanKind;
using rtype::server::BanList;
using rtype::server::BanTarget;
using rtype::server::testing::BAN_MOMENT;
using rtype::server::testing::BANNED_ADDRESS;
using rtype::server::testing::BANNED_NICKNAME;
using rtype::server::testing::BANNED_NICKNAME_IN_SMALL_LETTERS;
using rtype::server::testing::FREE_ADDRESS;
using rtype::server::testing::FREE_NICKNAME;
using rtype::server::testing::HOUR_BAN_END;
using rtype::server::testing::LONG_AFTER;
using rtype::server::testing::ONE_SECOND;

/**
 * Given a ban for good of the nickname ACE
 * When a pilot named ace, from another address, is judged
 * Then the ban bars it: nicknames are compared without the case of ASCII
 * letters
 */
TEST(BanList, BarsTheNicknameWhateverTheCaseOfItsLetters) {
  const BanList bans{
      std::vector{Ban{.target = BanTarget{.kind = BanKind::Nickname,
                                          .text = std::string{BANNED_NICKNAME}},
                      .bannedAt = BAN_MOMENT,
                      .expiresAt = std::nullopt}}};

  EXPECT_NE(
      bans.barring(BanCandidate{.nickname = BANNED_NICKNAME_IN_SMALL_LETTERS,
                                .address = FREE_ADDRESS,
                                .at = BAN_MOMENT}),
      nullptr);
}

/**
 * Given a ban for good of an address
 * When a pilot of another nickname from that address is judged
 * Then the ban bars it
 */
TEST(BanList, BarsTheAddress) {
  const BanList bans{
      std::vector{Ban{.target = BanTarget{.kind = BanKind::Address,
                                          .text = std::string{BANNED_ADDRESS}},
                      .bannedAt = BAN_MOMENT,
                      .expiresAt = std::nullopt}}};

  EXPECT_NE(bans.barring(BanCandidate{.nickname = FREE_NICKNAME,
                                      .address = BANNED_ADDRESS,
                                      .at = BAN_MOMENT}),
            nullptr);
}

/**
 * Given a ban for good of an address
 * When a pilot from another address is judged
 * Then no ban bars it
 */
TEST(BanList, LetsAnotherAddressThrough) {
  const BanList bans{
      std::vector{Ban{.target = BanTarget{.kind = BanKind::Address,
                                          .text = std::string{BANNED_ADDRESS}},
                      .bannedAt = BAN_MOMENT,
                      .expiresAt = std::nullopt}}};

  EXPECT_EQ(bans.barring(BanCandidate{.nickname = FREE_NICKNAME,
                                      .address = FREE_ADDRESS,
                                      .at = BAN_MOMENT}),
            nullptr);
}

/**
 * Given a ban of ACE that ends an hour after it began
 * When ACE is judged one second before that end, then at that end
 * Then the ban bars it before its end, and no longer at it
 */
TEST(BanList, ABanEndsAtItsEnd) {
  const BanList bans{
      std::vector{Ban{.target = BanTarget{.kind = BanKind::Nickname,
                                          .text = std::string{BANNED_NICKNAME}},
                      .bannedAt = BAN_MOMENT,
                      .expiresAt = HOUR_BAN_END}}};

  EXPECT_NE(bans.barring(BanCandidate{.nickname = BANNED_NICKNAME,
                                      .address = FREE_ADDRESS,
                                      .at = HOUR_BAN_END - ONE_SECOND}),
            nullptr);
  EXPECT_EQ(bans.barring(BanCandidate{.nickname = BANNED_NICKNAME,
                                      .address = FREE_ADDRESS,
                                      .at = HOUR_BAN_END}),
            nullptr);
}

/**
 * Given a ban for good of ACE
 * When ACE is judged ten years later
 * Then the ban still bars it
 */
TEST(BanList, ABanForGoodNeverEnds) {
  const BanList bans{
      std::vector{Ban{.target = BanTarget{.kind = BanKind::Nickname,
                                          .text = std::string{BANNED_NICKNAME}},
                      .bannedAt = BAN_MOMENT,
                      .expiresAt = std::nullopt}}};

  EXPECT_NE(bans.barring(BanCandidate{.nickname = BANNED_NICKNAME,
                                      .address = FREE_ADDRESS,
                                      .at = LONG_AFTER}),
            nullptr);
}

/**
 * Given a list holding a ban of an hour of ACE
 * When a ban for good of ace is put
 * Then the list holds one ban, the one for good
 */
TEST(BanList, PutReplacesTheBanOfTheSameTarget) {
  BanList bans{
      std::vector{Ban{.target = BanTarget{.kind = BanKind::Nickname,
                                          .text = std::string{BANNED_NICKNAME}},
                      .bannedAt = BAN_MOMENT,
                      .expiresAt = HOUR_BAN_END}}};

  bans.put(
      Ban{.target =
              BanTarget{.kind = BanKind::Nickname,
                        .text = std::string{BANNED_NICKNAME_IN_SMALL_LETTERS}},
          .bannedAt = BAN_MOMENT,
          .expiresAt = std::nullopt});

  ASSERT_EQ(bans.bans().size(), 1U);
  EXPECT_FALSE(bans.bans().front().expiresAt.has_value());
}

/**
 * Given a list holding a ban of ACE
 * When the ban of ace is removed twice
 * Then the first removal says a ban was there, the second that none was
 */
TEST(BanList, RemoveSaysWhetherABanWasThere) {
  BanList bans{
      std::vector{Ban{.target = BanTarget{.kind = BanKind::Nickname,
                                          .text = std::string{BANNED_NICKNAME}},
                      .bannedAt = BAN_MOMENT,
                      .expiresAt = std::nullopt}}};
  const BanTarget target{.kind = BanKind::Nickname,
                         .text = std::string{BANNED_NICKNAME_IN_SMALL_LETTERS}};

  EXPECT_TRUE(bans.remove(target));
  EXPECT_FALSE(bans.remove(target));
}

/**
 * Given a ban of the nickname that reads as an address
 * When a pilot of another nickname from that address is judged
 * Then no ban bars it: a nickname ban never names an address
 */
TEST(BanList, ANicknameBanNamesNoAddress) {
  const BanList bans{
      std::vector{Ban{.target = BanTarget{.kind = BanKind::Nickname,
                                          .text = std::string{BANNED_ADDRESS}},
                      .bannedAt = BAN_MOMENT,
                      .expiresAt = std::nullopt}}};

  EXPECT_EQ(bans.barring(BanCandidate{.nickname = FREE_NICKNAME,
                                      .address = BANNED_ADDRESS,
                                      .at = BAN_MOMENT}),
            nullptr);
}
