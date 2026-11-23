#include <gtest/gtest.h>
#include <filesystem>
#include <iterator>
#include <optional>
#include <string>
#include "AccountTestConstants.hpp"
#include "SessionTokenFile.hpp"
#include "SessionTokenFileConstants.hpp"
#include "SessionTokenFileTestConstants.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::SESSION_TOKEN_FILE_NAME;
using rtype::client::SessionTokenFile;
using rtype::client::testing::ACE_TOKEN;
using rtype::client::testing::NEW_SETTINGS_FOLDER;
using rtype::client::testing::OTHER_TOKEN;
using rtype::client::testing::TOO_LARGE_SESSION_FILE_SIZE;

/**
 * Given no session file yet
 * When the token is loaded
 * Then there is none
 */
TEST(SessionTokenFileTest, NoFileHoldsNoToken) {
  const TemporaryFolder folder;
  const SessionTokenFile file(folder.path() / SESSION_TOKEN_FILE_NAME);

  EXPECT_FALSE(file.load().has_value());
}

/**
 * Given the token of ACE kept
 * When a new launch loads the same file
 * Then it gets that token
 */
TEST(SessionTokenFileTest, AKeptTokenIsLoadedAtTheNextLaunch) {
  const TemporaryFolder folder;
  SessionTokenFile(folder.path() / SESSION_TOKEN_FILE_NAME)
      .keep(std::string{ACE_TOKEN});

  const SessionTokenFile next(folder.path() / SESSION_TOKEN_FILE_NAME);

  EXPECT_EQ(next.load(), std::optional<std::string>{ACE_TOKEN});
}

/**
 * Given a session file in a folder that does not exist yet
 * When a token is kept
 * Then the folder and the file are created
 */
TEST(SessionTokenFileTest, TheFirstKeepCreatesTheFolder) {
  const TemporaryFolder folder;
  const std::filesystem::path path =
      folder.path() / NEW_SETTINGS_FOLDER / SESSION_TOKEN_FILE_NAME;
  SessionTokenFile file(path);

  file.keep(std::string{ACE_TOKEN});

  EXPECT_TRUE(std::filesystem::is_regular_file(path));
}

/**
 * Given the token of ACE kept
 * When another token is kept over it
 * Then the file holds the second one, and no temporary file is left
 */
TEST(SessionTokenFileTest, ASecondKeepReplacesTheFirst) {
  const TemporaryFolder folder;
  const std::filesystem::path path = folder.path() / SESSION_TOKEN_FILE_NAME;
  SessionTokenFile file(path);
  file.keep(std::string{ACE_TOKEN});

  file.keep(std::string{OTHER_TOKEN});

  EXPECT_EQ(file.load(), std::optional<std::string>{OTHER_TOKEN});
  EXPECT_EQ(std::distance(std::filesystem::directory_iterator(folder.path()),
                          std::filesystem::directory_iterator{}),
            1);
}

/**
 * Given the token of ACE kept
 * When it is forgotten
 * Then the next launch finds no token, and no file
 */
TEST(SessionTokenFileTest, ForgetRemovesTheFile) {
  const TemporaryFolder folder;
  const std::filesystem::path path = folder.path() / SESSION_TOKEN_FILE_NAME;
  SessionTokenFile file(path);
  file.keep(std::string{ACE_TOKEN});

  file.forget();

  EXPECT_FALSE(std::filesystem::exists(path));
}

/**
 * Given a session file with no path, as with `--session`
 * When a token is kept
 * Then nothing is kept
 */
TEST(SessionTokenFileTest, AFileWithNoPathKeepsNothing) {
  SessionTokenFile file(std::nullopt);

  file.keep(std::string{ACE_TOKEN});

  EXPECT_FALSE(file.load().has_value());
}

/**
 * Given an empty session file
 * When the token is loaded
 * Then there is none
 */
TEST(SessionTokenFileTest, AnEmptyFileHoldsNoToken) {
  const TemporaryFolder folder;
  folder.write(SESSION_TOKEN_FILE_NAME, "");
  const SessionTokenFile file(folder.path() / SESSION_TOKEN_FILE_NAME);

  EXPECT_FALSE(file.load().has_value());
}

/**
 * Given a session file larger than the client reads
 * When the token is loaded
 * Then the file is ignored
 */
TEST(SessionTokenFileTest, ATooLargeFileIsIgnored) {
  const TemporaryFolder folder;
  folder.write(SESSION_TOKEN_FILE_NAME,
               std::string(TOO_LARGE_SESSION_FILE_SIZE, 'a'));
  const SessionTokenFile file(folder.path() / SESSION_TOKEN_FILE_NAME);

  EXPECT_FALSE(file.load().has_value());
}

/**
 * Given a token kept
 * When the permissions of the file are read
 * Then only the player may read or write it
 */
TEST(SessionTokenFileTest, TheFileIsPrivateToThePlayer) {
#ifdef _WIN32
  GTEST_SKIP() << "Windows keeps the file in the private folder of the user";
#else
  const TemporaryFolder folder;
  const std::filesystem::path path = folder.path() / SESSION_TOKEN_FILE_NAME;
  SessionTokenFile file(path);

  file.keep(std::string{ACE_TOKEN});

  EXPECT_EQ(
      std::filesystem::status(path).permissions(),
      std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
#endif
}
