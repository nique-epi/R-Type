#include <gtest/gtest.h>
#include <filesystem>
#include "JournalFolder.hpp"
#include "JournalFolderTestConstants.hpp"

using rtype::client::journalFolder;

#ifdef __APPLE__

/**
 * Given macOS
 * When the journal folder is asked for
 * Then it is the absolute Logs folder of the user's Library
 */
TEST(JournalFolderTest, MacOsKeepsTheJournalInTheLogsOfTheLibrary) {
  const std::filesystem::path folder =
      journalFolder().value_or(std::filesystem::path());

  EXPECT_TRUE(folder.is_absolute());
  EXPECT_EQ(folder.filename(), MACOS_LOGS_FOLDER);
  EXPECT_EQ(folder.parent_path().filename(), MACOS_LIBRARY_FOLDER);
}

#else

/**
 * Given Linux or Windows
 * When the journal folder is asked for
 * Then there is none, so the journal goes next to the executable
 */
TEST(JournalFolderTest, OtherSystemsKeepTheJournalNextToTheExecutable) {
  EXPECT_FALSE(journalFolder().has_value());
}

#endif
