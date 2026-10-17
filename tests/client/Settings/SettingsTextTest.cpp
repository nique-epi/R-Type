#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <utility>
#include <vector>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "PixelSize.hpp"
#include "PlayerSettings.hpp"
#include "SettingsText.hpp"

using rtype::client::BoundAction;
using rtype::client::ColorMode;
using rtype::client::PixelSize;
using rtype::client::PlayerSettings;
using rtype::client::readSettingsText;
using rtype::client::settingsText;
using Scan = sf::Keyboard::Scan;

namespace {

/** @brief The file of a first launch, as the format of the options writes
 * it: a comment, the version, one line per setting. */
constexpr const char* FIRST_LAUNCH_FILE =
    "# R-Type settings, written by the game\n"
    "version=1\n"
    "music_volume=70\n"
    "effects_volume=80\n"
    "mute_all=off\n"
    "key_move_up=UP\n"
    "key_move_down=DOWN\n"
    "key_move_left=LEFT\n"
    "key_move_right=RIGHT\n"
    "key_shoot=SPACE\n"
    "key_ready=R\n"
    "color_mode=off\n"
    "reduce_flashes=off\n"
    "sound_cues=off\n"
    "auto_fire=off\n";

constexpr int ANY_VOLUME = 40;
constexpr int QUIET_MUSIC = 35;
constexpr int QUIET_EFFECTS = 5;
constexpr PixelSize SMALLER_WINDOW{.width = 1280, .height = 720};

}  // namespace

/**
 * Given the settings of a first launch
 * When they are written
 * Then the text is the file of a first launch, line for line
 */
TEST(SettingsTextTest, AFirstLaunchWritesTheDocumentedFile) {
  EXPECT_EQ(settingsText(PlayerSettings{}), FIRST_LAUNCH_FILE);
}

/**
 * Given every setting changed from its first launch value
 * When the settings are written then read back
 * Then they are the settings written
 */
TEST(SettingsTextTest, EveryChangedSettingReadsBack) {
  PlayerSettings settings;
  settings.musicVolume = QUIET_MUSIC;
  settings.effectsVolume = QUIET_EFFECTS;
  settings.muted = true;
  settings.windowSize = SMALLER_WINDOW;
  settings.keys.bind(BoundAction::Shoot, Scan::LControl);
  settings.colorMode = ColorMode::Tritanopia;
  settings.reduceFlashes = true;
  settings.soundCues = true;
  settings.autoFire = true;

  EXPECT_EQ(readSettingsText(settingsText(settings), PlayerSettings{}).settings,
            settings);
}

/**
 * Given a file whose music volume is out of range and whose effects volume
 * is right
 * When it is read
 * Then the music keeps its default, the effects take the file's volume, and
 * line 1 is listed as invalid
 */
TEST(SettingsTextTest, AnInvalidValueKeepsTheDefaultOfThatSettingOnly) {
  const auto reading = readSettingsText("music_volume=101\neffects_volume=40\n",
                                        PlayerSettings{});
  EXPECT_EQ(reading.settings.musicVolume, 70);
  EXPECT_EQ(reading.settings.effectsVolume, ANY_VOLUME);
  EXPECT_EQ(reading.ignoredLines,
            std::vector<std::string>{"line 1: invalid value"});
}

/**
 * Given volumes at each side of the range, 0 and 100 included
 * When each is read
 * Then -1 and 101 are refused, 0 and 100 are taken
 */
TEST(SettingsTextTest, AVolumeIsTakenFromZeroToOneHundred) {
  const std::vector<std::pair<std::string, int>> cases{
      {"music_volume=-1", 70},
      {"music_volume=0", 0},
      {"music_volume=100", 100},
      {"music_volume=101", 70}};
  for (const auto& [text, volume] : cases) {
    EXPECT_EQ(readSettingsText(text, PlayerSettings{}).settings.musicVolume,
              volume)
        << text;
  }
}

/**
 * Given a line naming no setting
 * When it is read
 * Then it is listed as unknown and changes nothing
 */
TEST(SettingsTextTest, AnUnknownSettingIsListed) {
  const auto reading = readSettingsText("brightness=3", PlayerSettings{});
  EXPECT_EQ(reading.settings, PlayerSettings{});
  EXPECT_EQ(reading.ignoredLines,
            std::vector<std::string>{"line 1: unknown setting"});
}

/**
 * Given a line with no equals sign
 * When it is read
 * Then it is listed as not NAME=VALUE
 */
TEST(SettingsTextTest, ALineWithoutEqualsSignIsListed) {
  EXPECT_EQ(readSettingsText("music_volume 40", PlayerSettings{}).ignoredLines,
            std::vector<std::string>{"line 1: not NAME=VALUE"});
}

/**
 * Given a file edited by hand on Windows: a comment, spaces around the name
 * and the value, CRLF line ends and an empty line
 * When it is read
 * Then the volume is taken and no line is listed
 */
TEST(SettingsTextTest, SpacesLineEndsAndCommentsAreIgnored) {
  const auto reading = readSettingsText(
      "# mine\r\n  music_volume = 40 \r\n\r\n", PlayerSettings{});
  EXPECT_EQ(reading.settings.musicVolume, ANY_VOLUME);
  EXPECT_TRUE(reading.ignoredLines.empty());
}

/**
 * Given a key line naming no key the player may bind
 * When it is read
 * Then the action keeps its key and the line is listed as invalid
 */
TEST(SettingsTextTest, AnUnknownKeyNameKeepsTheKey) {
  const auto reading = readSettingsText("key_shoot=SPACEBAR", PlayerSettings{});
  EXPECT_EQ(reading.settings.keys.keyOf(BoundAction::Shoot), Scan::Space);
  EXPECT_EQ(reading.ignoredLines,
            std::vector<std::string>{"line 1: invalid value"});
}

/**
 * Given window sizes that are not two positive whole numbers joined by x
 * When each is read
 * Then none is taken
 */
TEST(SettingsTextTest, AWindowSizeNeedsTwoPositiveWholeNumbers) {
  for (const char* text :
       {"window_size=0x720", "window_size=1280x", "window_size=1280*720",
        "window_size=1280x720x1", "window_size=-1280x720"}) {
    EXPECT_FALSE(readSettingsText(text, PlayerSettings{})
                     .settings.windowSize.has_value())
        << text;
  }
}

/**
 * Given a file of another version
 * When it is read
 * Then its version line is listed, and its settings are still read
 */
TEST(SettingsTextTest, AnotherVersionIsListedAndItsSettingsRead) {
  const auto reading =
      readSettingsText("version=2\nmusic_volume=40\n", PlayerSettings{});
  EXPECT_EQ(reading.settings.musicVolume, ANY_VOLUME);
  EXPECT_EQ(reading.ignoredLines,
            std::vector<std::string>{"line 1: invalid value"});
}

/**
 * Given an empty text
 * When it is read
 * Then the defaults come back and no line is listed
 */
TEST(SettingsTextTest, AnEmptyTextGivesTheDefaults) {
  const auto reading = readSettingsText("", PlayerSettings{});
  EXPECT_EQ(reading.settings, PlayerSettings{});
  EXPECT_TRUE(reading.ignoredLines.empty());
}
