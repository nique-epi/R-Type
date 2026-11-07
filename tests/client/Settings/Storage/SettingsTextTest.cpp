#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <utility>
#include <vector>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "DisplayMode.hpp"
#include "Endpoint.hpp"
#include "PlayerSettings.hpp"
#include "SettingsText.hpp"
#include "SettingsTextTestConstants.hpp"

using rtype::client::BoundAction;
using rtype::client::ColorMode;
using rtype::client::DisplayMode;
using rtype::client::PlayerSettings;
using rtype::client::readSettingsText;
using rtype::client::settingsText;
using rtype::client::testing::ANY_VOLUME;
using rtype::client::testing::CHANGED_CHAT_BACKGROUND;
using rtype::client::testing::CHANGED_CHAT_CLOSED_LINES;
using rtype::client::testing::CHANGED_CHAT_LETTERS;
using rtype::client::testing::CHANGED_CHAT_OPEN_LINES;
using rtype::client::testing::CHANGED_CHAT_SECONDS;
using rtype::client::testing::CHANGED_EFFECTS_VOLUME;
using rtype::client::testing::CHANGED_MUSIC_VOLUME;
using rtype::client::testing::CHAT_LINES_PAST_THEIR_RANGES;
using rtype::client::testing::CHAT_PAST_ITS_RANGES_FILE;
using rtype::client::testing::CHOSEN_MASTER_ADDRESS;
using rtype::client::testing::CHOSEN_MASTER_LINE;
using rtype::client::testing::CHOSEN_MASTER_PORT;
using rtype::client::testing::FIRST_LAUNCH_FILE;
using rtype::client::testing::FIRST_LAUNCH_MUSIC_VOLUME;
using rtype::client::testing::FIRST_LINE_WITHOUT_SEPARATOR;
using rtype::client::testing::HIGHEST_VOLUME;
using rtype::client::testing::HOST_NAME_MASTER_LINE;
using rtype::client::testing::INVALID_FIRST_LINE;
using rtype::client::testing::LOWEST_VOLUME;
using rtype::client::testing::SMALLER_WINDOW;
using rtype::client::testing::UNKNOWN_FIRST_LINE;
using rtype::network::Endpoint;
using Scan = sf::Keyboard::Scan;

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
  settings.musicVolume = CHANGED_MUSIC_VOLUME;
  settings.effectsVolume = CHANGED_EFFECTS_VOLUME;
  settings.muted = true;
  settings.windowSize = SMALLER_WINDOW;
  settings.displayMode = DisplayMode::Borderless;
  settings.keys.bind(BoundAction::Shoot, Scan::LControl);
  settings.colorMode = ColorMode::Tritanopia;
  settings.reduceFlashes = true;
  settings.soundCues = true;
  settings.autoFire = true;
  settings.clientPrediction = false;
  settings.pilotNames = false;
  settings.keys.bind(BoundAction::OpenChat, Scan::Y);
  settings.chatShown = false;
  settings.chatLetters = CHANGED_CHAT_LETTERS;
  settings.chatClosedLines = CHANGED_CHAT_CLOSED_LINES;
  settings.chatOpenLines = CHANGED_CHAT_OPEN_LINES;
  settings.chatSeconds = CHANGED_CHAT_SECONDS;
  settings.chatBackground = CHANGED_CHAT_BACKGROUND;
  settings.master = Endpoint{.address = std::string{CHOSEN_MASTER_ADDRESS},
                             .port = CHOSEN_MASTER_PORT};

  EXPECT_EQ(readSettingsText(settingsText(settings), PlayerSettings{}).settings,
            settings);
}

/**
 * Given a file whose five chat numbers each lie one past their range
 * When it is read over the settings of a first launch
 * Then the chat keeps its first launch look, and the five lines are listed
 */
TEST(SettingsTextTest, AChatValuePastItsRangeKeepsItsDefault) {
  const auto reading =
      readSettingsText(CHAT_PAST_ITS_RANGES_FILE, PlayerSettings{});

  EXPECT_EQ(reading.settings, PlayerSettings{});
  EXPECT_EQ(reading.ignoredLines.size(), CHAT_LINES_PAST_THEIR_RANGES);
}

/**
 * Given the line a chosen master is kept on
 * When it is read over the settings of a first launch
 * Then the master is that address and that port
 */
TEST(SettingsTextTest, AMasterReadsBackAsItsAddressAndPort) {
  const auto reading = readSettingsText(CHOSEN_MASTER_LINE, PlayerSettings{});

  EXPECT_EQ(reading.settings.master.value_or(Endpoint{}),
            (Endpoint{.address = std::string{CHOSEN_MASTER_ADDRESS},
                      .port = CHOSEN_MASTER_PORT}));
}

/**
 * Given a master line that names a host, not an IPv4 address
 * When it is read
 * Then no master is taken, and the line is listed as invalid
 */
TEST(SettingsTextTest, AMasterWithAHostNameIsLeftOut) {
  const auto reading =
      readSettingsText(HOST_NAME_MASTER_LINE, PlayerSettings{});

  EXPECT_FALSE(reading.settings.master.has_value());
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{INVALID_FIRST_LINE});
}

/**
 * Given a file written before the pilot names existed, prediction off
 * When it is read over the settings of a first launch
 * Then prediction is off as the file says, and the pilot names are shown
 */
TEST(SettingsTextTest, AFileWithoutPilotNamesShowsThem) {
  const auto reading =
      readSettingsText("client_prediction=off\n", PlayerSettings{});
  EXPECT_TRUE(reading.settings.pilotNames);
  EXPECT_FALSE(reading.settings.clientPrediction);
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
  EXPECT_EQ(reading.settings.musicVolume, FIRST_LAUNCH_MUSIC_VOLUME);
  EXPECT_EQ(reading.settings.effectsVolume, ANY_VOLUME);
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{INVALID_FIRST_LINE});
}

/**
 * Given volumes at each side of the range, 0 and 100 included
 * When each is read
 * Then -1 and 101 are refused, 0 and 100 are taken
 */
TEST(SettingsTextTest, AVolumeIsTakenFromZeroToOneHundred) {
  const std::vector<std::pair<std::string, int>> cases{
      {"music_volume=-1", FIRST_LAUNCH_MUSIC_VOLUME},
      {"music_volume=0", LOWEST_VOLUME},
      {"music_volume=100", HIGHEST_VOLUME},
      {"music_volume=101", FIRST_LAUNCH_MUSIC_VOLUME}};
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
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{UNKNOWN_FIRST_LINE});
}

/**
 * Given a line with no equals sign
 * When it is read
 * Then it is listed as not NAME=VALUE
 */
TEST(SettingsTextTest, ALineWithoutEqualsSignIsListed) {
  EXPECT_EQ(readSettingsText("music_volume 40", PlayerSettings{}).ignoredLines,
            std::vector<std::string>{FIRST_LINE_WITHOUT_SEPARATOR});
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
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{INVALID_FIRST_LINE});
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
 * Given a display mode line naming no display mode
 * When it is read
 * Then the window stays windowed, and the line is listed as invalid
 */
TEST(SettingsTextTest, AnUnknownDisplayModeKeepsTheWindowWindowed) {
  const auto reading =
      readSettingsText("display_mode=fullscreen", PlayerSettings{});
  EXPECT_EQ(reading.settings.displayMode, DisplayMode::Windowed);
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{INVALID_FIRST_LINE});
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
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{INVALID_FIRST_LINE});
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
