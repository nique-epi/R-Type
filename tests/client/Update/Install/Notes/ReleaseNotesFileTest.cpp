#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include <string>
#include "InstallConstants.hpp"
#include "InstallFixtures.hpp"
#include "InstallTestConstants.hpp"
#include "ReleaseNotes.hpp"
#include "ReleaseNotesFile.hpp"

using rtype::client::readReleaseNotes;
using rtype::client::RELEASE_NOTES_FILE_NAME;
using rtype::client::ReleaseNotes;
using rtype::client::writeReleaseNotes;
using rtype::client::testing::BROKEN_NOTES_FOLDER;
using rtype::client::testing::emptyFolder;
using rtype::client::testing::MISSING_NOTES_FOLDER;
using rtype::client::testing::NOTE_LINE;
using rtype::client::testing::NOTES_OF_A_NUMBER;
using rtype::client::testing::NOTES_VERSION;
using rtype::client::testing::writeFile;
using rtype::client::testing::WRITTEN_NOTES_FOLDER;

/**
 * Given notes written to a file
 * When the file is read
 * Then the same version and lines come back
 */
TEST(ReleaseNotesFile, ReadsWhatItWrote) {
  const std::filesystem::path file =
      emptyFolder(WRITTEN_NOTES_FOLDER) / RELEASE_NOTES_FILE_NAME;
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
  const std::filesystem::path file =
      emptyFolder(MISSING_NOTES_FOLDER) / RELEASE_NOTES_FILE_NAME;

  EXPECT_FALSE(readReleaseNotes(file).has_value());
}

/**
 * Given a notes file whose line is a number
 * When it is read
 * Then there are no notes
 */
TEST(ReleaseNotesFile, ReadsNothingFromABrokenFile) {
  const std::filesystem::path file =
      emptyFolder(BROKEN_NOTES_FOLDER) / RELEASE_NOTES_FILE_NAME;
  writeFile(file, NOTES_OF_A_NUMBER);

  EXPECT_FALSE(readReleaseNotes(file).has_value());
}
