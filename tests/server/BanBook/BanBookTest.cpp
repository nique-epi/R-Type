#include <SQLiteCpp/Exception.h>
#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include <vector>
#include "BanBook.hpp"
#include "BanBookFixture.hpp"
#include "BanFixtures.hpp"
#include "BanKind.hpp"
#include "BanTestConstants.hpp"
#include "BansDatabase.hpp"

namespace rtype::server::testing {

/**
 * Given a book that banned ACE for good, then was closed
 * When another book opens the same file, as the supervisor does when it
 * starts again
 * Then it holds that ban
 */
TEST_F(BanBookFixture, KeepsABanAfterItIsReopened) {
  BanBook first;
  first.open(file(), BAN_MOMENT);
  first.put(banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt));
  first.close();
  BanBook second;

  second.open(file(), BAN_MOMENT);

  EXPECT_EQ(
      second.list().bans(),
      (std::vector{banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt)}));
}

/**
 * Given a file with a ban of an hour of ACE and a ban for good of an address
 * When a book opens it once the hour ended
 * Then it holds the ban for good only
 */
TEST_F(BanBookFixture, ReadsOnlyTheBansInForceAtOpen) {
  BansDatabase writer{file()};
  writer.save(banOf(BanKind::Nickname, BANNED_NICKNAME, HOUR_BAN_END));
  writer.save(banOf(BanKind::Address, BANNED_ADDRESS, std::nullopt));
  BanBook book;

  book.open(file(), HOUR_BAN_END);

  EXPECT_EQ(
      book.list().bans(),
      (std::vector{banOf(BanKind::Address, BANNED_ADDRESS, std::nullopt)}));
}

/**
 * Given a book open on an existing file whose table another connection then
 * dropped
 * When it is asked to ban ACE
 * Then it throws what SQLite refused, and its list still holds no ban
 */
TEST_F(BanBookFixture, ARefusedWriteLeavesTheListAsItWas) {
  const BansDatabase creator{file()};
  BanBook book;
  book.open(file(), BAN_MOMENT);
  dropTheBans();

  EXPECT_THROW(
      book.put(banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt)),
      SQLite::Exception);

  EXPECT_TRUE(book.list().bans().empty());
}

/**
 * Given a book open on a file that does not exist
 * When nothing is banned, then ACE is
 * Then the file does not exist before the ban, and exists after it
 */
TEST_F(BanBookFixture, CreatesNoFileUntilTheFirstBan) {
  BanBook book;
  book.open(file(), BAN_MOMENT);
  ASSERT_FALSE(std::filesystem::exists(file()));

  book.put(banOf(BanKind::Nickname, BANNED_NICKNAME, std::nullopt));

  EXPECT_TRUE(std::filesystem::exists(file()));
}

}  // namespace rtype::server::testing
