#include <gtest/gtest.h>
#include <array>
#include <optional>
#include "Caption.hpp"
#include "ColorMode.hpp"
#include "Entity.hpp"
#include "InterfaceColorConstants.hpp"
#include "Layer.hpp"
#include "PilotNameTags.hpp"
#include "PilotNameTagsFixture.hpp"
#include "PilotNameTagsTestConstants.hpp"
#include "PlayerPaletteConstants.hpp"
#include "Position.hpp"
#include "TextStyle.hpp"

using rtype::client::Caption;
using rtype::client::ColorMode;
using rtype::client::nameTagOffsetOf;
using rtype::client::PLAYER_COLORS;
using rtype::client::TextFont;
using rtype::client::TRITANOPIA_PLAYER_PALETTE;
using rtype::engine::Entity;
using rtype::game::Layer;
using rtype::game::Position;

/**
 * Given a ship 16 pixels tall in the middle of the playfield
 * When the offset of the tag ACE is asked
 * Then the tag is centered 56 units above the ship, across from its center
 */
TEST(PilotNameTags, ATagStandsOverItsShip) {
  const Position offset =
      nameTagOffsetOf(MIDDLE_OF_THE_PLAYFIELD, SHIP_FRAME, SECOND_PILOT_NAME);

  EXPECT_EQ(offset.x, 0.0F);
  EXPECT_EQ(offset.y, -RISE_OVER_THE_SHIP);
}

/**
 * Given a ship at y 76, whose tag starts at y 0, then a ship at y 75
 * When the offsets of their tags are asked
 * Then the first tag stands above its ship, the second 56 units below it
 */
TEST(PilotNameTags, ATagGoesUnderAShipWithNoRoomAbove) {
  const Position roomAbove = nameTagOffsetOf(
      {.x = MIDDLE_OF_THE_PLAYFIELD.x, .y = LOWEST_Y_WITH_ROOM_ABOVE},
      SHIP_FRAME, SECOND_PILOT_NAME);
  const Position noRoomAbove = nameTagOffsetOf(
      {.x = MIDDLE_OF_THE_PLAYFIELD.x, .y = HIGHEST_Y_WITHOUT_ROOM_ABOVE},
      SHIP_FRAME, SECOND_PILOT_NAME);

  EXPECT_EQ(roomAbove.y, -RISE_OVER_THE_SHIP);
  EXPECT_EQ(noRoomAbove.y, RISE_OVER_THE_SHIP);
}

/**
 * Given ships at x 64, 259, 260 and 1856, each tagged with a name of 16
 * characters, 260 units each side of its center
 * When the offsets of their tags are asked
 * Then the tags move in by 196 and 1 units, not at all, and by 196 units the
 * other way, so each line stays whole within the playfield
 */
TEST(PilotNameTags, ATagStaysWholeNearTheSides) {
  const auto offsetAt = [](float shipX) {
    return nameTagOffsetOf({.x = shipX, .y = MIDDLE_OF_THE_PLAYFIELD.y},
                           SHIP_FRAME, LONGEST_NAME)
        .x;
  };

  EXPECT_EQ(offsetAt(SHIP_AT_THE_LEFT_EDGE), MOVE_IN_FROM_THE_LEFT_EDGE);
  EXPECT_EQ(offsetAt(SHIP_ONE_UNIT_TOO_FAR_LEFT), MOVE_IN_BY_ONE_UNIT);
  EXPECT_EQ(offsetAt(SHIP_WITH_JUST_ENOUGH_ROOM), 0.0F);
  EXPECT_EQ(offsetAt(SHIP_AT_THE_RIGHT_EDGE), MOVE_IN_FROM_THE_RIGHT_EDGE);
}

/**
 * Given ships at x 64 and 1856, each tagged with a name of 8 characters of 2
 * bytes, which the small font may draw up to 10 pixels wide each
 * When the offsets of their tags are asked
 * Then the tags move in by 196 units, as for 16 characters of ASCII, so the
 * line stays whole within the playfield
 */
TEST(PilotNameTags, ANameOutsideAsciiStaysWholeNearTheSides) {
  const auto offsetAt = [](float shipX) {
    return nameTagOffsetOf({.x = shipX, .y = MIDDLE_OF_THE_PLAYFIELD.y},
                           SHIP_FRAME, LONGEST_NAME_OUTSIDE_ASCII)
        .x;
  };

  EXPECT_EQ(offsetAt(SHIP_AT_THE_LEFT_EDGE), MOVE_IN_FROM_THE_LEFT_EDGE);
  EXPECT_EQ(offsetAt(SHIP_AT_THE_RIGHT_EDGE), MOVE_IN_FROM_THE_RIGHT_EDGE);
}

/**
 * Given the ship of pilot 2, ACE, PILOT NAMES on as at a first launch
 * When the ships are tagged
 * Then the ship carries ACE, shown, outlined, in the small font, in the light
 * shade of color 1, in the Popups layer, 56 units above it
 */
TEST_F(PilotNameTagsFixture, AShipCarriesTheNameOfItsPilot) {
  const Entity ship = spawnShip(SECOND_PILOT_COLOR, MIDDLE_OF_THE_PLAYFIELD);
  const std::array pilots{pilotNamed(SECOND_PILOT, SECOND_PILOT_NAME)};

  tag(pilots);

  const std::optional<Caption> found = tagOf(ship);
  ASSERT_TRUE(found.has_value());
  const Caption shown = found.value_or(Caption{});
  EXPECT_EQ(shown.text, SECOND_PILOT_NAME);
  EXPECT_TRUE(shown.visible);
  EXPECT_TRUE(shown.style.outlined);
  EXPECT_EQ(shown.style.font, TextFont::Small);
  EXPECT_EQ(shown.style.color, PLAYER_COLORS.at(SECOND_PILOT_COLOR).light);
  EXPECT_EQ(shown.layer, Layer::Popups);
  EXPECT_EQ(shown.offset.y, -RISE_OVER_THE_SHIP);
}

/**
 * Given the ship of pilot 3, whom the waiting room never named
 * When the ships are tagged
 * Then the ship carries P3, as the status band shows that pilot
 */
TEST_F(PilotNameTagsFixture, AnUnnamedPilotIsShownByItsNumber) {
  const Entity ship = spawnShip(THIRD_PILOT_COLOR, MIDDLE_OF_THE_PLAYFIELD);
  const std::array pilots{pilotNamed(THIRD_PILOT, "")};

  tag(pilots);

  const std::optional<Caption> found = tagOf(ship);
  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found.value_or(Caption{}).text, THIRD_PILOT_SHOWN_NAME);
}

/**
 * Given a tagged ship
 * When PILOT NAMES is set off and the ships are tagged, then set on again and
 * the ships are tagged
 * Then the tag is hidden after the first call and shown after the second
 */
TEST_F(PilotNameTagsFixture, TheSettingHidesAndShowsTheTag) {
  const Entity ship = spawnShip(SECOND_PILOT_COLOR, MIDDLE_OF_THE_PLAYFIELD);
  const std::array pilots{pilotNamed(SECOND_PILOT, SECOND_PILOT_NAME)};
  tag(pilots);

  settings().pilotNames = false;
  tag(pilots);
  const bool shownWhenOff = tagOf(ship).value_or(Caption{}).visible;
  settings().pilotNames = true;
  tag(pilots);
  const bool shownWhenOn = tagOf(ship).value_or(Caption{}).visible;

  EXPECT_FALSE(shownWhenOff);
  EXPECT_TRUE(shownWhenOn);
}

/**
 * Given a ship tagged in the colors of a first launch
 * When the color mode is set to tritanopia and the ships are tagged again
 * Then the tag takes the light shade the tritanopia palette gives color 1,
 * which is not the one it had
 */
TEST_F(PilotNameTagsFixture, TheTagFollowsThePalette) {
  const Entity ship = spawnShip(SECOND_PILOT_COLOR, MIDDLE_OF_THE_PLAYFIELD);
  const std::array pilots{pilotNamed(SECOND_PILOT, SECOND_PILOT_NAME)};
  tag(pilots);

  settings().colorMode = ColorMode::Tritanopia;
  tag(pilots);

  const auto tritanopia = TRITANOPIA_PLAYER_PALETTE.at(SECOND_PILOT_COLOR);
  ASSERT_NE(tritanopia.light, PLAYER_COLORS.at(SECOND_PILOT_COLOR).light);
  EXPECT_EQ(tagOf(ship).value_or(Caption{}).style.color, tritanopia.light);
}

/**
 * Given a tagged ship in the middle of the playfield
 * When it moves to y 75, where the tag has no room above it, and the ships
 * are tagged again
 * Then the tag goes under the ship
 */
TEST_F(PilotNameTagsFixture, ATagFollowsItsShip) {
  const Entity ship = spawnShip(SECOND_PILOT_COLOR, MIDDLE_OF_THE_PLAYFIELD);
  const std::array pilots{pilotNamed(SECOND_PILOT, SECOND_PILOT_NAME)};
  tag(pilots);

  moveShip(ship,
           {.x = MIDDLE_OF_THE_PLAYFIELD.x, .y = HIGHEST_Y_WITHOUT_ROOM_ABOVE});
  tag(pilots);

  EXPECT_EQ(tagOf(ship).value_or(Caption{}).offset.y, RISE_OVER_THE_SHIP);
}

/**
 * Given the ship of pilot 4, while the board lists pilot 2 only
 * When the ships are tagged
 * Then that ship carries no tag
 */
TEST_F(PilotNameTagsFixture, AShipWithoutPilotGetsNoTag) {
  const Entity ship = spawnShip(FOURTH_PILOT_COLOR, MIDDLE_OF_THE_PLAYFIELD);
  const std::array pilots{pilotNamed(SECOND_PILOT, SECOND_PILOT_NAME)};

  tag(pilots);

  EXPECT_FALSE(tagOf(ship).has_value());
}
