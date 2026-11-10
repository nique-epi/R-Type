#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include <string>
#include "ReleaseNotes.hpp"
#include "ReleaseNotesFile.hpp"
#include "RunTestConstants.hpp"
#include "TemporaryFolder.hpp"
#include "UpdateConstants.hpp"

using rtype::client::readReleaseNotes;
using rtype::client::RELEASE_NOTES_FILE_NAME;
using rtype::client::ReleaseNotes;
using rtype::client::writeReleaseNotes;
using rtype::client::testing::NOTE_LINE;
using rtype::client::testing::NOTES_OF_A_NUMBER;
using rtype::client::testing::NOTES_VERSION;

/**
 * Given notes written to a file
 * When the file is read
 * Then the same version and lines come back
 */
TEST(ReleaseNotesFile, ReadsWhatItWrote) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / RELEASE_NOTES_FILE_NAME;
  const ReleaseNotes written{.versionText = std::string{NOTES_VERSION},
                             .lines = {std::string{NOTE_LINE}}};

  ASSERT_TRUE(writeReleaseNotes(file, written));
  const std::optional<ReleaseNotes> read = readReleaseNotes(file);

  ASSERT_TRUE(read.has_value());
  const ReleaseNotes notes = read.value_or(ReleaseNotes{});
  EXPECT_EQ(notes.versionText, NOTES_VERSION);
  EXPECT_EQ(notes.lines, written.lines);
}

/**
 * Given no notes file
 * When it is read
 * Then there are no notes
 */
TEST(ReleaseNotesFile, ReadsNothingFromAMissingFile) {
  const TemporaryFolder folder;
  const std::filesystem::path file = folder.path() / RELEASE_NOTES_FILE_NAME;

  EXPECT_FALSE(readReleaseNotes(file).has_value());
}

/**
 * Given a notes file whose line is a number
 * When it is read
 * Then there are no notes
 */
TEST(ReleaseNotesFile, ReadsNothingFromABrokenFile) {
  const TemporaryFolder folder;
  folder.write(RELEASE_NOTES_FILE_NAME, NOTES_OF_A_NUMBER);
  const std::filesystem::path file = folder.path() / RELEASE_NOTES_FILE_NAME;

  EXPECT_FALSE(readReleaseNotes(file).has_value());
}
