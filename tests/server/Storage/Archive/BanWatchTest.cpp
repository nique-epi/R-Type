#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stop_token>
#include "BanFixtures.hpp"
#include "BanKind.hpp"
#include "BanList.hpp"
#include "BanTestConstants.hpp"
#include "BanWatch.hpp"
#include "BanWatchTestConstants.hpp"
#include "BansDatabase.hpp"
#include "BansDatabaseFixture.hpp"

namespace rtype::server::testing {

/**
 * Given a file that holds a ban for good of ACE
 * When a watch opens it
 * Then the bans it publishes hold that ban as soon as open() returned
 */
TEST_F(BansDatabaseFixture, AWatchPublishesTheBansAtOpen) {
  BansDatabase{file()}.save(
      banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt));
  BanWatch watch;

  watch.open(file());

  const BanList bans = watch.bans().value().value_or(BanList{});
  EXPECT_EQ(bans.bans().size(), 1U);
  watch.close();
}

/**
 * Given a watch open on a file without bans
 * When a ban for good is written to the file afterwards
 * Then the watch publishes bans that hold it, within BAN_WATCH_WAIT; a reread
 * published before the write holds no ban, so the test waits for one that does
 */
TEST_F(BansDatabaseFixture, AWatchPublishesABanWrittenAfterItOpened) {
  BanWatch watch;
  watch.open(file());
  std::uint64_t seen = watch.bans().revision();

  BansDatabase{file()}.save(
      banOf(BanKind::Address, BANNED_ADDRESS, std::nullopt));
  const auto deadline = std::chrono::steady_clock::now() + BAN_WATCH_WAIT;
  std::size_t published = 0;
  while (published == 0 && std::chrono::steady_clock::now() < deadline) {
    seen = watch.bans().waitForNewer(seen, BAN_WATCH_WAIT, std::stop_token{});
    published = watch.bans().value().value_or(BanList{}).bans().size();
  }

  EXPECT_EQ(published, 1U);
  watch.close();
}

/**
 * Given a watch open on a file that holds a ban for good of ACE
 * When the watch reads the file again, twice, and finds the same ban
 * Then it publishes nothing more: the revision the lobby follows only moves
 * when the bans change
 */
TEST_F(BansDatabaseFixture, AWatchPublishesNothingWhenTheBansDidNotChange) {
  BansDatabase{file()}.save(
      banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt));
  BanWatch watch;
  watch.open(file());
  const std::uint64_t opened = watch.bans().revision();

  const std::uint64_t after = watch.bans().waitForNewer(
      opened, TWO_REREADS_AND_A_HALF, std::stop_token{});

  EXPECT_EQ(after, opened);
  watch.close();
}

}  // namespace rtype::server::testing
