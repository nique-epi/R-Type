#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "PilotStatus.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "StatusBand.hpp"
#include "StatusBandFixtures.hpp"
#include "StatusBandTestConstants.hpp"
#include "TextStyle.hpp"

using rtype::client::paintStatusBand;
using rtype::client::PilotState;
using rtype::client::PilotStatus;
using rtype::client::TextAlignment;
using rtype::client::TextFont;
using rtype::client::testing::BAND_BOTTOM;
using rtype::client::testing::BAND_TOP;
using rtype::client::testing::DIMMED;
using rtype::client::testing::DIMMED_TEXT;
using rtype::client::testing::FIRST_CHIP;
using rtype::client::testing::FIRST_FRAME_CORNERS;
using rtype::client::testing::FIRST_LIFE_SOURCE;
using rtype::client::testing::FIRST_LIGHT;
using rtype::client::testing::FIRST_LIVES;
using rtype::client::testing::FIRST_LIVES_COUNT;
using rtype::client::testing::FIRST_NAME;
using rtype::client::testing::FIRST_PLUG;
using rtype::client::testing::FIRST_SCORE_END;
using rtype::client::testing::FIRST_SMALL_WORD_END;
using rtype::client::testing::FIRST_WORD_END;
using rtype::client::testing::FIRST_ZEROS_END;
using rtype::client::testing::FIVE_LIVES;
using rtype::client::testing::FOUR_LIVES;
using rtype::client::testing::FOURTH_MARKER_SOURCE;
using rtype::client::testing::LARGEST_SHOWN_SCORE_TEXT;
using rtype::client::testing::LAST_LIFE;
using rtype::client::testing::LEADING_ZEROS;
using rtype::client::testing::LIVES_SHOWN_AT_MOST;
using rtype::client::testing::LOCAL_FRAME;
using rtype::client::testing::LONGEST_NICKNAME;
using rtype::client::testing::LONGEST_NICKNAME_BESIDE_A_WORD;
using rtype::client::testing::LONGEST_NICKNAME_CUT;
using rtype::client::testing::LOST_LIFE_SOURCE;
using rtype::client::testing::MIDDLE_ROW;
using rtype::client::testing::NAME_AND_SCORE;
using rtype::client::testing::NICKNAME_WITH_ACCENTS;
using rtype::client::testing::NICKNAME_WITH_ACCENTS_CUT;
using rtype::client::testing::NINE_DIGIT_SCORE;
using rtype::client::testing::NO_LIFE;
using rtype::client::testing::NO_PLAYER;
using rtype::client::testing::PAST_THE_LAST_PLAYER;
using rtype::client::testing::PICTURE_WIDTH;
using rtype::client::testing::PLAYER_FOUR;
using rtype::client::testing::PLAYER_ONE;
using rtype::client::testing::PLAYER_THREE;
using rtype::client::testing::PLUG_ICON;
using rtype::client::testing::RULE_COLUMNS;
using rtype::client::testing::RULE_HEIGHT;
using rtype::client::testing::RULE_TOP;
using rtype::client::testing::THIRD_CHIP;
using rtype::client::testing::THIRD_COLOR;
using rtype::client::testing::THIRD_LIFE_SOURCE;
using rtype::client::testing::THREE_LIVES;
using rtype::client::testing::TWO_LIVES;
using rtype::client::testing::WARNING;
using ::testing::Contains;
using ::testing::IsEmpty;

/**
 * Given no pilot
 * When the band is painted
 * Then it covers every column of its middle row and nothing outside rows 246
 * to 269
 */
TEST(StatusBand, CoversTheBottom24RowsOfThePicture) {
  const InterfaceCanvasRecord record = bandPaintingOf({});

  const std::set<std::pair<int, int>> lit = litPixelsOf(record);
  for (int column = 0; column < PICTURE_WIDTH; ++column) {
    EXPECT_TRUE(lit.contains({column, MIDDLE_ROW})) << column;
  }
  EXPECT_TRUE(std::ranges::all_of(lit, [](const std::pair<int, int>& pixel) {
    return pixel.second >= BAND_TOP && pixel.second <= BAND_BOTTOM;
  }));
}

/**
 * Given no pilot
 * When the band is painted
 * Then a rule 16 rows tall separates each two bays, and nothing is written
 */
TEST(StatusBand, RulesSeparateEmptyBays) {
  const InterfaceCanvasRecord record = bandPaintingOf({});

  for (const int column : RULE_COLUMNS) {
    EXPECT_THAT(
        record.fills,
        Contains(RecordedFill{.area = {{column, RULE_TOP}, {1, RULE_HEIGHT}},
                              .color = DIMMED}));
  }
  EXPECT_THAT(record.writings, IsEmpty());
}

/**
 * Given player 3 flying
 * When the band is painted
 * Then its chip, filled with its color, sits in the third bay
 */
TEST(StatusBand, APilotTakesTheBayOfItsNumber) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_THREE, THREE_LIVES)});

  EXPECT_THAT(record.fills,
              Contains(RecordedFill{.area = THIRD_CHIP, .color = THIRD_COLOR}));
}

/**
 * Given player 1, called ACE, flying
 * When the band is painted
 * Then ACE titles its bay in the regular font, in the light shade of its
 * color, and no P1 is written
 */
TEST(StatusBand, TheNicknameTitlesTheBay) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_ONE, THREE_LIVES)});

  const RecordedWriting* name = writingOf(record, "ACE");
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->anchor, FIRST_NAME);
  EXPECT_EQ(name->style.font, TextFont::Regular);
  EXPECT_EQ(name->style.color, FIRST_LIGHT);
  EXPECT_EQ(writingOf(record, "P1"), nullptr);
}

/**
 * Given player 1 with a nickname of 16 characters and no word
 * When the band is painted
 * Then its first 11 characters are written
 */
TEST(StatusBand, ALongNicknameIsCutToTheBay) {
  PilotStatus pilot = flying(PLAYER_ONE, THREE_LIVES);
  pilot.nickname = LONGEST_NICKNAME;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_NE(writingOf(record, LONGEST_NICKNAME_CUT), nullptr);
}

/**
 * Given player 1 respawning, with a nickname of 16 characters
 * When the band is painted
 * Then RESPAWN ends the first row and the nickname keeps the room left
 */
TEST(StatusBand, AWordShortensTheNickname) {
  PilotStatus pilot = flying(PLAYER_ONE, THREE_LIVES);
  pilot.nickname = LONGEST_NICKNAME;
  pilot.state = PilotState::Respawning;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_NE(writingOf(record, LONGEST_NICKNAME_BESIDE_A_WORD), nullptr);
  EXPECT_EQ(anchorOf(record, "RESPAWN"), FIRST_SMALL_WORD_END);
}

/**
 * Given player 1 whose nickname holds characters of two bytes
 * When the band is painted
 * Then it is cut after 11 whole characters, never inside one
 */
TEST(StatusBand, ANicknameIsCutAtAWholeCharacter) {
  PilotStatus pilot = flying(PLAYER_ONE, THREE_LIVES);
  pilot.nickname = NICKNAME_WITH_ACCENTS;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_NE(writingOf(record, NICKNAME_WITH_ACCENTS_CUT), nullptr);
}

/**
 * Given player 4 whom the waiting room never named
 * When the band is painted
 * Then P4 titles its bay
 */
TEST(StatusBand, AnUnnamedPilotShowsItsNumber) {
  PilotStatus pilot = flying(PLAYER_FOUR, THREE_LIVES);
  pilot.nickname.clear();

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_NE(writingOf(record, "P4"), nullptr);
}

/**
 * Given player 4 flying
 * When the band is painted
 * Then its marker is the star, the fourth marker of the sheet
 */
TEST(StatusBand, TheFourthPilotWearsTheStar) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_FOUR, THREE_LIVES)});

  EXPECT_EQ(imagesOf(record, FOURTH_MARKER_SOURCE).size(), 1U);
}

/**
 * Given player 1 with 12400 points
 * When the band is painted
 * Then 12400 ends the second row, and two dim zeros make it seven digits
 */
TEST(StatusBand, TheScoreIsPaddedToSevenDigits) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_ONE, THREE_LIVES)});

  const RecordedWriting* digits = writingOf(record, "12400");
  const RecordedWriting* zeros = writingOf(record, "00");
  ASSERT_NE(digits, nullptr);
  ASSERT_NE(zeros, nullptr);
  EXPECT_EQ(digits->anchor, FIRST_SCORE_END);
  EXPECT_EQ(digits->style.alignment, TextAlignment::Right);
  EXPECT_EQ(zeros->anchor, FIRST_ZEROS_END);
  EXPECT_EQ(zeros->style.color, LEADING_ZEROS);
}

/**
 * Given player 1 with a score of nine digits
 * When the band is painted
 * Then the score stops at the largest number of seven digits, 9999999, with no
 * zero added, so it never runs over the lives
 */
TEST(StatusBand, AScoreOverSevenDigitsStopsAtTheLargest) {
  PilotStatus pilot = flying(PLAYER_ONE, THREE_LIVES);
  pilot.score = NINE_DIGIT_SCORE;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_EQ(anchorOf(record, LARGEST_SHOWN_SCORE_TEXT), FIRST_SCORE_END);
  EXPECT_EQ(record.writings.size(), NAME_AND_SCORE);
}

/**
 * Given player 1 with 3 lives
 * When the band is painted
 * Then three ships of its color stand 17 pixels apart on the second row, and
 * no life is shown lost
 */
TEST(StatusBand, LivesShowOneShipEachUpToThree) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_ONE, THREE_LIVES)});

  const std::vector<RecordedImage> lives = imagesOf(record, FIRST_LIFE_SOURCE);
  ASSERT_EQ(lives.size(), FIRST_LIVES.size());
  for (std::size_t life = 0; life < lives.size(); ++life) {
    EXPECT_EQ(lives[life].topLeft, FIRST_LIVES.at(life));
  }
  EXPECT_THAT(imagesOf(record, LOST_LIFE_SOURCE), IsEmpty());
}

/**
 * Given player 1, a teammate, down to 1 life
 * When the band is painted
 * Then its ship stands in the first slot and the two lives it lost in the
 * other two, so the slots stay the ones an empty row shows
 */
TEST(StatusBand, LostLivesStayInTheirSlotsAsLostLives) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_ONE, LAST_LIFE)});

  const std::vector<RecordedImage> ships = imagesOf(record, FIRST_LIFE_SOURCE);
  const std::vector<RecordedImage> lost = imagesOf(record, LOST_LIFE_SOURCE);
  ASSERT_EQ(ships.size(), 1U);
  ASSERT_EQ(lost.size(), 2U);
  EXPECT_EQ(ships.front().topLeft, FIRST_LIVES.at(0));
  EXPECT_EQ(lost.front().topLeft, FIRST_LIVES.at(1));
  EXPECT_EQ(lost.back().topLeft, FIRST_LIVES.at(2));
}

/**
 * Given player 3 with 3 lives
 * When the band is painted
 * Then its ships come from its own color
 */
TEST(StatusBand, LivesTakeTheColorOfThePilot) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_THREE, THREE_LIVES)});

  EXPECT_EQ(imagesOf(record, THIRD_LIFE_SOURCE).size(), LIVES_SHOWN_AT_MOST);
}

/**
 * Given player 1 with 2 lives, one under the limit of three slots
 * When the band is painted
 * Then two ships fill the first two slots and a lost life the third
 */
TEST(StatusBand, TwoLivesShowTwoShipsAndOneLostLife) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_ONE, TWO_LIVES)});

  const std::vector<RecordedImage> ships = imagesOf(record, FIRST_LIFE_SOURCE);
  const std::vector<RecordedImage> lost = imagesOf(record, LOST_LIFE_SOURCE);
  ASSERT_EQ(ships.size(), 2U);
  ASSERT_EQ(lost.size(), 1U);
  EXPECT_EQ(ships.front().topLeft, FIRST_LIVES.at(0));
  EXPECT_EQ(ships.back().topLeft, FIRST_LIVES.at(1));
  EXPECT_EQ(lost.front().topLeft, FIRST_LIVES.at(2));
}

/**
 * Given player 1 with 4 lives, one over the limit of three slots
 * When the band is painted
 * Then one ship is drawn, followed by x4, and no life is shown lost
 */
TEST(StatusBand, FourLivesShowOneShipAndACount) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_ONE, FOUR_LIVES)});

  EXPECT_EQ(imagesOf(record, FIRST_LIFE_SOURCE).size(), 1U);
  EXPECT_EQ(anchorOf(record, "x4"), FIRST_LIVES_COUNT);
  EXPECT_THAT(imagesOf(record, LOST_LIFE_SOURCE), IsEmpty());
}

/**
 * Given player 1 with 5 lives
 * When the band is painted
 * Then one ship is drawn, followed by x5, and no life is shown lost
 */
TEST(StatusBand, MoreThanThreeLivesShowOneShipAndACount) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_ONE, FIVE_LIVES)});

  EXPECT_EQ(imagesOf(record, FIRST_LIFE_SOURCE).size(), 1U);
  EXPECT_EQ(anchorOf(record, "x5"), FIRST_LIVES_COUNT);
  EXPECT_THAT(imagesOf(record, LOST_LIFE_SOURCE), IsEmpty());
}

/**
 * Given player 1 whose lives the server has not listed yet
 * When the band is painted
 * Then no ship and no lost life are drawn, so it does not read as a pilot
 * out of lives
 */
TEST(StatusBand, UnknownLivesShowNoShip) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(PLAYER_ONE, std::nullopt)});

  EXPECT_THAT(imagesOf(record, FIRST_LIFE_SOURCE), IsEmpty());
  EXPECT_THAT(imagesOf(record, LOST_LIFE_SOURCE), IsEmpty());
}

/**
 * Given player 1 out of lives
 * When the band is painted
 * Then three lives lost are drawn, OUT ends the first row in orange, and its
 * chip is dimmed
 */
TEST(StatusBand, APilotOutOfLivesShowsLivesLostAndOut) {
  PilotStatus pilot = flying(PLAYER_ONE, NO_LIFE);
  pilot.state = PilotState::OutOfLives;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_EQ(imagesOf(record, LOST_LIFE_SOURCE).size(), LIVES_SHOWN_AT_MOST);
  const RecordedWriting* out = writingOf(record, "OUT");
  ASSERT_NE(out, nullptr);
  EXPECT_EQ(out->anchor, FIRST_WORD_END);
  EXPECT_EQ(out->style.color, WARNING);
  EXPECT_THAT(record.fills,
              Contains(RecordedFill{.area = FIRST_CHIP, .color = DIMMED}));
}

/**
 * Given the local player, player 1, with 3 lives
 * When the band is painted
 * Then its bay is framed in white and YOU ends its first row
 */
TEST(StatusBand, TheLocalPilotIsFramedAndTaggedYou) {
  PilotStatus pilot = flying(PLAYER_ONE, THREE_LIVES);
  pilot.isLocalPlayer = true;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  const std::set<std::pair<int, int>> frame = pixelsIn(record, LOCAL_FRAME);
  for (const std::pair<int, int>& corner : FIRST_FRAME_CORNERS) {
    EXPECT_TRUE(frame.contains(corner));
  }
  EXPECT_EQ(anchorOf(record, "YOU"), FIRST_WORD_END);
}

/**
 * Given the local player on its last life
 * When the band is painted
 * Then LAST LIFE is written in orange small print, in place of YOU
 */
TEST(StatusBand, LastLifeComesBeforeYou) {
  PilotStatus pilot = flying(PLAYER_ONE, LAST_LIFE);
  pilot.isLocalPlayer = true;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  const RecordedWriting* warning = writingOf(record, "LAST LIFE");
  ASSERT_NE(warning, nullptr);
  EXPECT_EQ(warning->anchor, FIRST_SMALL_WORD_END);
  EXPECT_EQ(warning->style.font, TextFont::Small);
  EXPECT_EQ(warning->style.color, WARNING);
  EXPECT_EQ(writingOf(record, "YOU"), nullptr);
}

/**
 * Given player 1 respawning, with 3 lives
 * When the band is painted
 * Then RESPAWN ends its first row in small print
 */
TEST(StatusBand, ARespawningPilotSaysSo) {
  PilotStatus pilot = flying(PLAYER_ONE, THREE_LIVES);
  pilot.state = PilotState::Respawning;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_EQ(anchorOf(record, "RESPAWN"), FIRST_SMALL_WORD_END);
}

/**
 * Given player 1 who left the game
 * When the band is painted
 * Then LEFT ends its first row, its score stays, dimmed, and no life is drawn
 */
TEST(StatusBand, APilotWhoLeftKeepsItsScore) {
  PilotStatus pilot = flying(PLAYER_ONE, THREE_LIVES);
  pilot.state = PilotState::Left;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_EQ(anchorOf(record, "LEFT"), FIRST_SMALL_WORD_END);
  const RecordedWriting* score = writingOf(record, "12400");
  ASSERT_NE(score, nullptr);
  EXPECT_EQ(score->style.color, DIMMED_TEXT);
  EXPECT_THAT(imagesOf(record, FIRST_LIFE_SOURCE), IsEmpty());
}

/**
 * Given player 1 whose signal is lost
 * When the band is painted
 * Then SIGNAL LOST ends its first row, and the unplugged plug is laid
 * smoothed over 8 x 8 pixels in place of its lives, tinted pale blue
 */
TEST(StatusBand, ALostSignalShowsTheUnpluggedPlug) {
  PilotStatus pilot = flying(PLAYER_ONE, THREE_LIVES);
  pilot.state = PilotState::SignalLost;

  const InterfaceCanvasRecord record = bandPaintingOf({pilot});

  EXPECT_EQ(anchorOf(record, "SIGNAL LOST"), FIRST_SMALL_WORD_END);
  ASSERT_EQ(record.smoothImages.size(), 1U);
  EXPECT_EQ(record.smoothImages.front().assetId, PLUG_ICON);
  EXPECT_EQ(record.smoothImages.front().area, FIRST_PLUG);
  EXPECT_EQ(record.smoothImages.front().tint, DIMMED_TEXT);
}

/**
 * Given pilots numbered 0 and 5, one step outside each bound of the bays
 * When the band is painted
 * Then neither is painted
 */
TEST(StatusBand, APilotWithoutABayIsNotPainted) {
  const InterfaceCanvasRecord record =
      bandPaintingOf({flying(NO_PLAYER, THREE_LIVES),
                      flying(PAST_THE_LAST_PLAYER, THREE_LIVES)});

  EXPECT_THAT(record.writings, IsEmpty());
  EXPECT_THAT(record.images, IsEmpty());
}
