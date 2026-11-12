#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <utility>
#include <vector>
#include "BoundAction.hpp"
#include "ColorMode.hpp"
#include "DisplayMode.hpp"
#include "PlayerSettings.hpp"
#include "SettingsText.hpp"
#include "SettingsTextTestConstants.hpp"

using rtype::client::BoundAction;
using rtype::client::ColorMode;
using rtype::client::DisplayMode;
using rtype::client::PlayerSettings;
using rtype::client::readSettingsText;
using rtype::client::settingsText;
using rtype::client::testing::A_BALANCE;
using rtype::client::testing::A_BALANCE_LINE;
using rtype::client::testing::ANY_VOLUME;
using rtype::client::testing::BALANCE_PAST_THE_LARGEST_FILE;
using rtype::client::testing::BARE_HEAD_FILE;
using rtype::client::testing::CHANGED_CHAT_BACKGROUND;
using rtype::client::testing::CHANGED_CHAT_CLOSED_LINES;
using rtype::client::testing::CHANGED_CHAT_LETTERS;
using rtype::client::testing::CHANGED_CHAT_OPEN_LINES;
using rtype::client::testing::CHANGED_CHAT_SECONDS;
using rtype::client::testing::CHANGED_EFFECTS_VOLUME;
using rtype::client::testing::CHANGED_LAGOMETER_BACKGROUND;
using rtype::client::testing::CHANGED_LAGOMETER_SCALE;
using rtype::client::testing::CHANGED_MUSIC_VOLUME;
using rtype::client::testing::CHAPKA_ITEM;
using rtype::client::testing::CHAT_LINES_PAST_THEIR_RANGES;
using rtype::client::testing::CHAT_PAST_ITS_RANGES_FILE;
using rtype::client::testing::CROWN_AND_SHADES_FILE;
using rtype::client::testing::CROWN_AS_A_HULL_FILE;
using rtype::client::testing::CROWN_LINE;
using rtype::client::testing::EARLIER_MASTER_LINE;
using rtype::client::testing::EARLIER_PREDICTION_LINE;
using rtype::client::testing::FILE_WITHOUT_A_HULL;
using rtype::client::testing::FIRST_LAUNCH_FILE;
using rtype::client::testing::FIRST_LAUNCH_MUSIC_VOLUME;
using rtype::client::testing::FIRST_LINE_WITHOUT_SEPARATOR;
using rtype::client::testing::FLIES_GOLDEN_ACE;
using rtype::client::testing::GOLDEN_ACE_LINE;
using rtype::client::testing::HIGHEST_VOLUME;
using rtype::client::testing::INVALID_FIRST_LINE;
using rtype::client::testing::LAGOMETER_LINES_PAST_THEIR_RANGES;
using rtype::client::testing::LAGOMETER_PAST_ITS_RANGES_FILE;
using rtype::client::testing::LOWEST_VOLUME;
using rtype::client::testing::NEGATIVE_BALANCE_FILE;
using rtype::client::testing::SHADES_ON_THE_HEAD_FILE;
using rtype::client::testing::SMALLER_WINDOW;
using rtype::client::testing::UNKNOWN_FIRST_LINE;
using rtype::client::testing::VOSTOK_AND_CHAPKA_LOCKER;
using rtype::client::testing::VOSTOK_ITEM;
using rtype::client::testing::WEARS_CROWN;
using rtype::client::testing::WEARS_NOTHING;
using rtype::client::testing::WEARS_SHADES;
using rtype::client::testing::WORDS_AS_A_BALANCE_FILE;
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
  settings.pilotNames = false;
  settings.keys.bind(BoundAction::OpenChat, Scan::Y);
  settings.chatShown = false;
  settings.chatLetters = CHANGED_CHAT_LETTERS;
  settings.chatClosedLines = CHANGED_CHAT_CLOSED_LINES;
  settings.chatOpenLines = CHANGED_CHAT_OPEN_LINES;
  settings.chatSeconds = CHANGED_CHAT_SECONDS;
  settings.chatBackground = CHANGED_CHAT_BACKGROUND;

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
 * Given settings whose lagometer is twice its size over a fill of 35 percent
 * When they are written, then read over the settings of a first launch
 * Then the settings read are the settings written
 */
TEST(SettingsTextTest, TheLagometerSettingsComeBackAsWritten) {
  PlayerSettings settings;
  settings.lagometerScale = CHANGED_LAGOMETER_SCALE;
  settings.lagometerBackground = CHANGED_LAGOMETER_BACKGROUND;

  EXPECT_EQ(readSettingsText(settingsText(settings), PlayerSettings{}).settings,
            settings);
}

/**
 * Given a file whose two lagometer numbers each lie one past their range
 * When it is read over the settings of a first launch
 * Then the lagometer keeps its first launch look, and the two lines are listed
 */
TEST(SettingsTextTest, ALagometerValuePastItsRangeKeepsItsDefault) {
  const auto reading =
      readSettingsText(LAGOMETER_PAST_ITS_RANGES_FILE, PlayerSettings{});

  EXPECT_EQ(reading.settings, PlayerSettings{});
  EXPECT_EQ(reading.ignoredLines.size(), LAGOMETER_LINES_PAST_THEIR_RANGES);
}

/**
 * Given the line an earlier version kept the master the player chose on
 * When it is read over the settings of a first launch
 * Then the line is listed as an unknown setting: the client reads only the
 * master the master-url setting names
 */
TEST(SettingsTextTest, AMasterLineOfAnEarlierVersionIsUnknown) {
  const auto reading = readSettingsText(EARLIER_MASTER_LINE, PlayerSettings{});

  EXPECT_EQ(reading.settings, PlayerSettings{});
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{UNKNOWN_FIRST_LINE});
}

/**
 * Given the line an earlier version kept the prediction switched off with
 * When it is read over the settings of a first launch
 * Then the line is listed as an unknown setting: the prediction is no setting
 */
TEST(SettingsTextTest, APredictionLineOfAnEarlierVersionIsUnknown) {
  const auto reading =
      readSettingsText(EARLIER_PREDICTION_LINE, PlayerSettings{});

  EXPECT_EQ(reading.settings, PlayerSettings{});
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{UNKNOWN_FIRST_LINE});
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

/**
 * Given settings whose ship wears the crown
 * When they are written
 * Then the line of the head names the crown
 */
TEST(SettingsTextTest, AWornItemIsWrittenByItsName) {
  PlayerSettings settings;
  settings.shipLook.head = WEARS_CROWN;

  EXPECT_NE(settingsText(settings).find(CROWN_LINE), std::string::npos);
}

/**
 * Given a file naming the crown on the head and the shades on the eyes
 * When it is read
 * Then the ship wears both, each in its slot
 */
TEST(SettingsTextTest, AWornItemReadsBackByItsName) {
  const PlayerSettings settings =
      readSettingsText(CROWN_AND_SHADES_FILE, PlayerSettings{}).settings;

  EXPECT_EQ(settings.shipLook.head, WEARS_CROWN);
  EXPECT_EQ(settings.shipLook.eyes, WEARS_SHADES);
}

/**
 * Given a file naming the shades on the head, a slot they never take
 * When it is read
 * Then the head keeps its default and the line is listed as invalid
 */
TEST(SettingsTextTest, AnItemOfAnotherSlotIsIgnored) {
  const auto reading =
      readSettingsText(SHADES_ON_THE_HEAD_FILE, PlayerSettings{});

  EXPECT_EQ(reading.settings.shipLook.head, WEARS_NOTHING);
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{INVALID_FIRST_LINE});
}

/**
 * Given defaults whose ship wears the crown
 * When a file naming none on the head is read
 * Then the head wears nothing
 */
TEST(SettingsTextTest, NoneTakesTheItemOff) {
  PlayerSettings defaults;
  defaults.shipLook.head = WEARS_CROWN;

  EXPECT_EQ(readSettingsText(BARE_HEAD_FILE, defaults).settings.shipLook.head,
            WEARS_NOTHING);
}

/**
 * Given the settings of a first launch
 * When they are read
 * Then the locker holds the starter kit: the top hat, the cool shades and the
 * mustache, and no other item
 */
TEST(SettingsTextTest, AFirstLaunchHoldsTheStarterKit) {
  const PlayerSettings settings;

  EXPECT_EQ(settings.shipLocker.items.count(), 3U);
  EXPECT_TRUE(settings.shipLocker.items.test(0));
  EXPECT_TRUE(settings.shipLocker.items.test(8));
  EXPECT_TRUE(settings.shipLocker.items.test(9));
}

/**
 * Given a locker holding the top hat, the crown, the shades and the mustache
 * When the settings are written
 * Then the line lists the items by their names, in the order of the catalog
 */
TEST(SettingsTextTest, TheLockerIsWrittenByItsItemNames) {
  PlayerSettings settings;
  settings.shipLocker.items.set(2);

  EXPECT_NE(settingsText(settings).find(
                "ship_locker=top_hat,crown,cool_shades,mustache\n"),
            std::string::npos);
}

/**
 * Given a file whose locker line names the crown and the shades
 * When it is read
 * Then the locker holds those two items alone
 */
TEST(SettingsTextTest, TheLockerReadsBackByItsNames) {
  const rtype::client::SettingsReading reading =
      readSettingsText("ship_locker=crown,cool_shades\n", PlayerSettings{});

  EXPECT_EQ(reading.settings.shipLocker.items.count(), 2U);
  EXPECT_TRUE(reading.settings.shipLocker.items.test(2));
  EXPECT_TRUE(reading.settings.shipLocker.items.test(8));
}

/**
 * Given a locker line that names the crown and an item no one knows
 * When it is read
 * Then the crown is in the locker and the unknown name is skipped
 */
TEST(SettingsTextTest, AnUnknownNameInTheLockerIsSkipped) {
  const rtype::client::SettingsReading reading =
      readSettingsText("ship_locker=crown,tiara\n", PlayerSettings{});

  EXPECT_EQ(reading.settings.shipLocker.items.count(), 1U);
  EXPECT_TRUE(reading.settings.shipLocker.items.test(2));
}

/**
 * Given a locker line holding none
 * When it is read
 * Then the locker is empty
 */
TEST(SettingsTextTest, NoneIsAnEmptyLocker) {
  const rtype::client::SettingsReading reading =
      readSettingsText("ship_locker=none\n", PlayerSettings{});

  EXPECT_TRUE(reading.settings.shipLocker.items.none());
}

/**
 * Given settings whose ship flies the golden ace
 * When they are written
 * Then the line of the hull names it
 */
TEST(SettingsTextTest, AHullIsWrittenByItsName) {
  PlayerSettings settings;
  settings.shipLook.hull = FLIES_GOLDEN_ACE;

  EXPECT_NE(settingsText(settings).find(GOLDEN_ACE_LINE), std::string::npos);
}

/**
 * Given a file naming the golden ace as the hull
 * When it is read
 * Then the ship flies it
 */
TEST(SettingsTextTest, AHullReadsBackByItsName) {
  EXPECT_EQ(readSettingsText(GOLDEN_ACE_LINE, PlayerSettings{})
                .settings.shipLook.hull,
            FLIES_GOLDEN_ACE);
}

/**
 * Given a file written before the hulls existed
 * When it is read
 * Then the ship flies the standard hull and no line is ignored
 */
TEST(SettingsTextTest, AFileWithoutAHullKeepsTheStandardShip) {
  const rtype::client::SettingsReading reading =
      readSettingsText(FILE_WITHOUT_A_HULL, PlayerSettings{});

  EXPECT_EQ(reading.settings.shipLook.hull, WEARS_NOTHING);
  EXPECT_TRUE(reading.ignoredLines.empty());
}

/**
 * Given a file naming the crown, an accessory, as the hull
 * When it is read
 * Then the hull keeps its default and the line is listed as invalid
 */
TEST(SettingsTextTest, AnAccessoryIsNoHull) {
  const rtype::client::SettingsReading reading =
      readSettingsText(CROWN_AS_A_HULL_FILE, PlayerSettings{});

  EXPECT_EQ(reading.settings.shipLook.hull, WEARS_NOTHING);
  EXPECT_EQ(reading.ignoredLines, std::vector<std::string>{INVALID_FIRST_LINE});
}

/**
 * Given a locker line naming a new accessory and a hull
 * When it is read
 * Then the locker holds both, and nothing else
 */
TEST(SettingsTextTest, TheLockerHoldsHullsAndNewAccessories) {
  const rtype::client::SettingsReading reading =
      readSettingsText(VOSTOK_AND_CHAPKA_LOCKER, PlayerSettings{});

  EXPECT_EQ(reading.settings.shipLocker.items.count(), 2U);
  EXPECT_TRUE(reading.settings.shipLocker.items.test(CHAPKA_ITEM));
  EXPECT_TRUE(reading.settings.shipLocker.items.test(VOSTOK_ITEM));
}

/**
 * Given settings whose player holds 1250 kopecks
 * When they are written, then read back
 * Then the file holds the line of the balance, and the balance reads back
 */
TEST(SettingsTextTest, TheBalanceIsWrittenAndReadBack) {
  PlayerSettings settings;
  settings.kopecks = A_BALANCE;

  EXPECT_NE(settingsText(settings).find(A_BALANCE_LINE), std::string::npos);
  EXPECT_EQ(readSettingsText(A_BALANCE_LINE, PlayerSettings{}).settings.kopecks,
            A_BALANCE);
}

/**
 * Given files whose balance is words, is negative, or passes the largest
 * When each is read
 * Then the player holds no kopeck and the line is listed as invalid
 */
TEST(SettingsTextTest, ABalanceThatIsNoBalanceLeavesNone) {
  for (const char* const file : {WORDS_AS_A_BALANCE_FILE, NEGATIVE_BALANCE_FILE,
                                 BALANCE_PAST_THE_LARGEST_FILE}) {
    SCOPED_TRACE(file);
    const rtype::client::SettingsReading reading =
        readSettingsText(file, PlayerSettings{});

    EXPECT_EQ(reading.settings.kopecks, 0U);
    EXPECT_EQ(reading.ignoredLines,
              std::vector<std::string>{INVALID_FIRST_LINE});
  }
}
