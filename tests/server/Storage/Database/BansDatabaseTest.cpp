#include <gtest/gtest.h>
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "Ban.hpp"
#include "BanKind.hpp"
#include "BanTarget.hpp"
#include "BanTestConstants.hpp"
#include "BansDatabase.hpp"
#include "BansDatabaseFixture.hpp"
#include "DatabaseFile.hpp"
#include "PathText.hpp"
#include "ResultsDatabaseConstants.hpp"

namespace rtype::server::testing {

/**
 * Given a ban of an hour written by a process that then closed the file
 * When another process opens the file
 * Then it reads that ban: the bans outlive the process that wrote them
 */
TEST_F(BansDatabaseFixture, KeepsABanForAProcessThatOpensTheFileLater) {
  const Ban ban = banOf(BanKind::Nickname, BANNED_NICKNAME, HOUR_BAN_END);
  {
    BansDatabase writer{file()};
    writer.save(ban);
  }

  const BansDatabase reader{file()};

  EXPECT_EQ(reader.inForce(BAN_MOMENT), std::vector{ban});
}

/**
 * Given a ban of an hour of an address
 * When a ban for good of the same address is saved
 * Then one ban is in force, the one for good
 */
TEST_F(BansDatabaseFixture, SavingTheSameTargetAgainReplacesItsBan) {
  BansDatabase bans{file()};
  bans.save(banOf(BanKind::Address, BANNED_ADDRESS, HOUR_BAN_END));

  bans.save(banOf(BanKind::Address, BANNED_ADDRESS, std::nullopt));

  EXPECT_EQ(bans.inForce(BAN_MOMENT),
            std::vector{banOf(BanKind::Address, BANNED_ADDRESS, std::nullopt)});
}

/**
 * Given a ban of ACE
 * When a ban of ace is saved
 * Then one ban is in force, written as the second one
 */
TEST_F(BansDatabaseFixture, ANicknameIsOneTargetWhateverItsCase) {
  BansDatabase bans{file()};
  bans.save(banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt));

  bans.save(
      banOf(BanKind::Nickname, BANNED_NICKNAME_IN_SMALL_LETTERS, std::nullopt));

  EXPECT_EQ(bans.inForce(BAN_MOMENT),
            std::vector{banOf(BanKind::Nickname,
                              BANNED_NICKNAME_IN_SMALL_LETTERS, std::nullopt)});
}

/**
 * Given a ban that ends at the end of the hour and one that ends a second later
 * When the bans in force at the end of the hour are read
 * Then only the second is listed: a ban past its end bars nobody
 */
TEST_F(BansDatabaseFixture, ListsNoBanThatEnded) {
  BansDatabase bans{file()};
  bans.save(banOf(BanKind::Nickname, BANNED_NICKNAME, HOUR_BAN_END));
  const Ban later =
      banOf(BanKind::Address, BANNED_ADDRESS, HOUR_BAN_END + ONE_SECOND);
  bans.save(later);

  EXPECT_EQ(bans.inForce(HOUR_BAN_END), std::vector{later});
}

/**
 * Given a ban of ACE
 * When the ban of ace is removed twice
 * Then the first removal says a ban was there, the second that none was, and
 * no ban is in force
 */
TEST_F(BansDatabaseFixture, RemoveSaysWhetherABanWasThere) {
  BansDatabase bans{file()};
  bans.save(banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt));
  const BanTarget target{.kind = BanKind::Nickname,
                         .text = std::string{BANNED_NICKNAME_IN_SMALL_LETTERS}};

  EXPECT_TRUE(bans.remove(target));
  EXPECT_FALSE(bans.remove(target));
  EXPECT_TRUE(bans.inForce(BAN_MOMENT).empty());
}

/**
 * Given a file made by a server that only knew the table of the results
 * When the bans open it, then save a ban
 * Then the ban is read back: the table of the bans was added to that file
 */
TEST_F(BansDatabaseFixture, AddsTheBansToAFileOfTheResultsOnly) {
  static_cast<void>(storage::openDatabaseFile(
      logging::genericText(file()),
      std::array<std::string_view, 1>{RESULTS_TABLE_MIGRATION}));
  BansDatabase bans{file()};

  bans.save(banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt));

  EXPECT_EQ(bans.inForce(BAN_MOMENT).size(), 1U);
}

}  // namespace rtype::server::testing
