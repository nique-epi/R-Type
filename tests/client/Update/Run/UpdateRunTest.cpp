#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <vector>
#include "FakeReleaseUpdater.hpp"
#include "ReleaseNotes.hpp"
#include "ReleaseNotesFile.hpp"
#include "RunFixtures.hpp"
#include "RunTestConstants.hpp"
#include "TemporaryFolder.hpp"
#include "UpdatePhase.hpp"
#include "UpdateRun.hpp"
#include "UpdateTestConstants.hpp"

using rtype::client::readReleaseNotes;
using rtype::client::ReleaseNotes;
using rtype::client::UpdatePhase;
using rtype::client::UpdateRun;
using rtype::client::testing::NOTES_VERSION;
using rtype::client::testing::reaches;
using rtype::client::testing::TAG_FIRST_NOTE;
using rtype::client::testing::TAG_SECOND_NOTE;
using rtype::client::testing::TAG_THIRD_NOTE;
using rtype::client::testing::updateOfTheTagNotes;
using rtype::client::testing::WHOLE_DOWNLOAD;

/**
 * Given a release whose download goes through
 * When the update is ready to restart
 * Then the panel reads the whole package received
 */
TEST(UpdateRun, ReportsTheWholePackageOnceDownloaded) {
  const TemporaryFolder folder;
  FakeReleaseUpdater updater;
  const UpdateRun run{updateOfTheTagNotes(updater, folder.path())};

  ASSERT_TRUE(reaches(run, UpdatePhase::ReadyToRestart));
  EXPECT_EQ(run.progress().percent, WHOLE_DOWNLOAD);
}

/**
 * Given a release whose download goes through
 * When the update is ready to restart
 * Then the notes of the tag are kept, as the panel shows them, for the next
 * start
 */
TEST(UpdateRun, KeepsTheNotesOfTheReleaseBeforeRestarting) {
  const TemporaryFolder folder;
  FakeReleaseUpdater updater;
  const auto parts = updateOfTheTagNotes(updater, folder.path());
  const std::filesystem::path notesFile = parts.notesFile;
  const UpdateRun run{parts};

  ASSERT_TRUE(reaches(run, UpdatePhase::ReadyToRestart));
  const ReleaseNotes notes =
      readReleaseNotes(notesFile).value_or(ReleaseNotes{});
  EXPECT_EQ(notes.versionText, NOTES_VERSION);
  EXPECT_EQ(notes.lines,
            (std::vector<std::string>{std::string{TAG_FIRST_NOTE},
                                      std::string{TAG_SECOND_NOTE},
                                      std::string{TAG_THIRD_NOTE}}));
}

/**
 * Given a release whose package is not the one its feed lists
 * When its download ends
 * Then the update failed, and no notes are kept
 */
TEST(UpdateRun, FailsWhenTheDownloadFails) {
  const TemporaryFolder folder;
  FakeReleaseUpdater updater;
  updater.failDownload();
  const auto parts = updateOfTheTagNotes(updater, folder.path());
  const std::filesystem::path notesFile = parts.notesFile;
  const UpdateRun run{parts};

  ASSERT_TRUE(reaches(run, UpdatePhase::Failed));
  EXPECT_FALSE(std::filesystem::exists(notesFile));
}

/**
 * Given an update ready to restart
 * When the panel restarts it
 * Then the updater of Velopack is started once
 */
TEST(UpdateRun, RestartStartsTheUpdater) {
  const TemporaryFolder folder;
  FakeReleaseUpdater updater;
  UpdateRun run{updateOfTheTagNotes(updater, folder.path())};
  ASSERT_TRUE(reaches(run, UpdatePhase::ReadyToRestart));

  EXPECT_TRUE(run.restart());
  EXPECT_EQ(updater.applyCount(), 1);
}

/**
 * Given an update ready to restart whose updater cannot start
 * When the panel restarts it
 * Then the update failed and its notes are dropped, so no WHAT'S NEW shows a
 * release that was not installed
 */
TEST(UpdateRun, AFailedRestartDropsTheNotes) {
  const TemporaryFolder folder;
  FakeReleaseUpdater updater;
  updater.failApply();
  const auto parts = updateOfTheTagNotes(updater, folder.path());
  const std::filesystem::path notesFile = parts.notesFile;
  UpdateRun run{parts};
  ASSERT_TRUE(reaches(run, UpdatePhase::ReadyToRestart));

  EXPECT_FALSE(run.restart());
  EXPECT_EQ(run.progress().phase, UpdatePhase::Failed);
  EXPECT_FALSE(std::filesystem::exists(notesFile));
}
