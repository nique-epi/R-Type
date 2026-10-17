#include <gtest/gtest.h>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include "PlayerSettings.hpp"
#include "SettingsFile.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::PlayerSettings;
using rtype::client::SettingsFile;

namespace {

/** @brief The largest file the client reads, in bytes, as the settings
 * format documents it. */
constexpr std::size_t LARGEST_FILE = 65536;
constexpr int SAVED_VOLUME = 35;
constexpr int OTHER_VOLUME = 40;

/** @returns The settings of a first launch, the music at a volume. */
PlayerSettings withMusicAt(int volume) {
  PlayerSettings settings;
  settings.musicVolume = volume;
  return settings;
}

/** @returns A file of a size: the music at OTHER_VOLUME, then a comment that
 * fills the rest. */
std::string fileOfSize(std::size_t size) {
  std::string text = "music_volume=" + std::to_string(OTHER_VOLUME) + "\n#";
  text.resize(size, '#');
  return text;
}

}  // namespace

/**
 * Given no file yet
 * When the settings are loaded
 * Then the defaults come back
 */
TEST(SettingsFileTest, NoFileGivesTheDefaults) {
  const TemporaryFolder folder;
  const SettingsFile file(folder.path() / "settings.ini");
  EXPECT_EQ(file.load(PlayerSettings{}), PlayerSettings{});
}

/**
 * Given settings saved
 * When a new launch loads the same file
 * Then it gets the settings saved
 */
TEST(SettingsFileTest, ASaveIsLoadedAtTheNextLaunch) {
  const TemporaryFolder folder;
  SettingsFile file(folder.path() / "settings.ini");
  ASSERT_TRUE(file.save(withMusicAt(SAVED_VOLUME)));
  EXPECT_EQ(SettingsFile(folder.path() / "settings.ini").load(PlayerSettings{}),
            withMusicAt(SAVED_VOLUME));
}

/**
 * Given a file in a folder that does not exist yet
 * When the settings are saved
 * Then the folder and the file are created
 */
TEST(SettingsFileTest, TheFirstSaveCreatesTheFolder) {
  const TemporaryFolder folder;
  SettingsFile file(folder.path() / "r-type" / "settings.ini");
  ASSERT_TRUE(file.save(PlayerSettings{}));
  EXPECT_TRUE(std::filesystem::is_regular_file(folder.path() / "r-type" /
                                               "settings.ini"));
}

/**
 * Given settings saved once
 * When other settings are saved over them
 * Then the file holds the second ones, and no temporary file is left
 */
TEST(SettingsFileTest, ASecondSaveReplacesTheFirst) {
  const TemporaryFolder folder;
  SettingsFile file(folder.path() / "settings.ini");
  ASSERT_TRUE(file.save(withMusicAt(SAVED_VOLUME)));
  ASSERT_TRUE(file.save(withMusicAt(OTHER_VOLUME)));
  EXPECT_EQ(file.load(PlayerSettings{}), withMusicAt(OTHER_VOLUME));
  EXPECT_FALSE(std::filesystem::exists(folder.path() / "settings.ini.tmp"));
}

/**
 * Given a file with no path, as when the system tells no settings folder
 * When the settings are saved
 * Then the save fails
 */
TEST(SettingsFileTest, AFileWithoutPathNeverSaves) {
  SettingsFile file(std::nullopt);
  EXPECT_FALSE(file.save(PlayerSettings{}));
}

/**
 * Given a regular file where the settings folder should be
 * When the settings are saved
 * Then the save fails and the file in the way is left as it was
 */
TEST(SettingsFileTest, ASaveIntoAFileInTheWayFails) {
  const TemporaryFolder folder;
  folder.write("blocked", "mine");
  SettingsFile file(folder.path() / "blocked" / "settings.ini");
  EXPECT_FALSE(file.save(PlayerSettings{}));
  EXPECT_TRUE(std::filesystem::is_regular_file(folder.path() / "blocked"));
}

/**
 * Given a file exactly as large as the client reads
 * When it is loaded
 * Then its volume is taken
 */
TEST(SettingsFileTest, AFileOfTheLargestSizeIsRead) {
  const TemporaryFolder folder;
  folder.write("settings.ini", fileOfSize(LARGEST_FILE));
  EXPECT_EQ(SettingsFile(folder.path() / "settings.ini")
                .load(PlayerSettings{})
                .musicVolume,
            OTHER_VOLUME);
}

/**
 * Given a file one byte larger than the client reads
 * When it is loaded
 * Then it is ignored and the defaults come back
 */
TEST(SettingsFileTest, AFileOneByteLargerIsIgnored) {
  const TemporaryFolder folder;
  folder.write("settings.ini", fileOfSize(LARGEST_FILE + 1));
  EXPECT_EQ(SettingsFile(folder.path() / "settings.ini").load(PlayerSettings{}),
            PlayerSettings{});
}

/**
 * Given a settings folder created by a save
 * When its permissions are read
 * Then only its owner may enter it, as the XDG specification asks
 */
TEST(SettingsFileTest, TheCreatedFolderBelongsToItsOwnerOnly) {
#ifdef _WIN32
  GTEST_SKIP() << "Windows folders have no POSIX permissions";
#else
  const TemporaryFolder folder;
  SettingsFile file(folder.path() / "r-type" / "settings.ini");
  ASSERT_TRUE(file.save(PlayerSettings{}));
  const std::filesystem::perms others =
      std::filesystem::perms::group_all | std::filesystem::perms::others_all;
  EXPECT_EQ(
      std::filesystem::status(folder.path() / "r-type").permissions() & others,
      std::filesystem::perms::none);
#endif
}
