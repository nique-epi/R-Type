#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <optional>
#include <vector>
#include "InterfaceColorConstants.hpp"
#include "MotionPreference.hpp"
#include "PlayerPaletteConstants.hpp"
#include "PlayerPaletteTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "Squadron.hpp"
#include "SquadronFixtures.hpp"
#include "SquadronTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::DEUTERANOPIA_PLAYER_PALETTE;
using rtype::client::EDGE_BLUE;
using rtype::client::MotionPreference;
using rtype::client::Squadron;
using rtype::client::testing::A_STALL;
using rtype::client::testing::A_WHILE;
using rtype::client::testing::ANY_SEED;
using rtype::client::testing::BOTTOM_ROW;
using rtype::client::testing::BOX;
using rtype::client::testing::CORNER_STROKE;
using rtype::client::testing::FIRST_MISSILE_OUT_NEXT_NOT_FIRED;
using rtype::client::testing::FRAME;
using rtype::client::testing::JUST_AFTER_THE_FIRST_ARRIVAL;
using rtype::client::testing::JUST_AFTER_THE_FIRST_SHOT;
using rtype::client::testing::JUST_BEFORE_THE_FIRST_SHOT;
using rtype::client::testing::LONG_WATCH;
using rtype::client::testing::MIDDLE_ROW;
using rtype::client::testing::MISSILE_SIZE;
using rtype::client::testing::ONCE_LANDED_BEFORE_THE_DRIFT;
using rtype::client::testing::ONCE_THE_BOX_IS_OPEN;
using rtype::client::testing::SHEET_GREEN_TO_BLUISH_GREEN;
using rtype::client::testing::SHIP_SIZE;
using rtype::client::testing::SHIPS_KEPT_IN_DEUTERANOPIA;
using rtype::client::testing::SHIPS_SWAPPED_IN_DEUTERANOPIA;
using rtype::client::testing::THIRD_SHIP_ROW;
using rtype::client::testing::THREE_LETTERS_IN;
using rtype::client::testing::TITLE_POSITION;
using rtype::client::testing::WHILE_THE_LINE_GROWS;
using rtype::engine::Duration;
using ::testing::Contains;
using ::testing::ElementsAreArray;
using ::testing::IsEmpty;
using ::testing::SizeIs;

/**
 * Given a squadron that arrived 90 milliseconds ago
 * When it is painted
 * Then a single line, one pixel high, lies on the middle row of the box,
 * centered
 */
TEST(Squadron, ALineGrowsFromTheMiddleAtFirst) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, WHILE_THE_LINE_GROWS);

  const InterfaceCanvasRecord painted = paintingOf(squadron);

  ASSERT_THAT(painted.fills, SizeIs(1));
  const sf::IntRect line = painted.fills.front().area;
  EXPECT_EQ(line.position.y, BOX.position.y + MIDDLE_ROW);
  EXPECT_EQ(line.size.y, 1);
  EXPECT_EQ(line.position.x - BOX.position.x,
            (BOX.position.x + BOX.size.x) - (line.position.x + line.size.x));
}

/**
 * Given a squadron that arrived 500 milliseconds ago
 * When it is painted
 * Then its corners lie on the top and bottom rows of the box
 */
TEST(Squadron, TheBoxIsOpenOnceTheOpeningEnds) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, ONCE_THE_BOX_IS_OPEN);

  const InterfaceCanvasRecord painted = paintingOf(squadron);

  EXPECT_THAT(painted.fills,
              Contains(RecordedFill{.area = {BOX.position, CORNER_STROKE},
                                    .color = EDGE_BLUE}));
  EXPECT_THAT(painted.fills,
              Contains(RecordedFill{
                  .area = {{BOX.position.x, BOX.position.y + BOTTOM_ROW},
                           CORNER_STROKE},
                  .color = EDGE_BLUE}));
}

/**
 * Given a squadron that arrived 575 milliseconds ago
 * When it is painted
 * Then the first three letters of SQUADRON are written
 */
TEST(Squadron, ItsTitleIsWrittenLetterByLetter) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, THREE_LETTERS_IN);

  const InterfaceCanvasRecord painted = paintingOf(squadron);

  const RecordedWriting* title = writingOf(painted, "SQU");
  ASSERT_NE(title, nullptr);
  EXPECT_EQ(title->anchor, TITLE_POSITION);
}

/**
 * Given a squadron whose first ship entered 10 milliseconds ago
 * When it is painted
 * Then that ship is still left of the box, cut off at its left edge
 */
TEST(Squadron, ShipsEnterThroughTheLeftEdgeOfTheBox) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, JUST_AFTER_THE_FIRST_ARRIVAL);

  const InterfaceCanvasRecord painted = paintingOf(squadron);

  ASSERT_THAT(cornersOf(painted, SHIP_SIZE), SizeIs(1));
  EXPECT_LT(cornersOf(painted, SHIP_SIZE).front().x, BOX.position.x);
  EXPECT_EQ(painted.images.front().visibleArea, BOX);
}

/**
 * Given a squadron whose four ships have flown in, before it drifts
 * When it is painted
 * Then each ship is at its place in the formation
 */
TEST(Squadron, FourShipsLandOnTheirPlaces) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, ONCE_LANDED_BEFORE_THE_DRIFT);

  const InterfaceCanvasRecord painted = paintingOf(squadron);

  EXPECT_THAT(cornersOf(painted, SHIP_SIZE),
              ElementsAreArray(formationInBox()));
}

/**
 * Given a squadron playing for 30 seconds, frame by frame
 * When it is painted at every frame once its ships have landed
 * Then every ship lies wholly inside the box
 */
TEST(Squadron, LandedShipsNeverLeaveTheBox) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, ONCE_LANDED_BEFORE_THE_DRIFT);
  bool alwaysInside = true;

  for (Duration played{}; played < LONG_WATCH; played += FRAME) {
    squadron.advance(FRAME);
    for (const sf::Vector2i corner :
         cornersOf(paintingOf(squadron), SHIP_SIZE)) {
      const sf::IntRect ship(corner, SHIP_SIZE);
      alwaysInside = alwaysInside && BOX.findIntersection(ship) == ship;
    }
  }

  EXPECT_TRUE(alwaysInside);
}

/**
 * Given a squadron that arrived 2390 milliseconds ago
 * When it is painted
 * Then no missile flies yet
 */
TEST(Squadron, NoShipFiresBeforeTheFireStart) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, JUST_BEFORE_THE_FIRST_SHOT);

  EXPECT_THAT(cornersOf(paintingOf(squadron), MISSILE_SIZE), IsEmpty());
}

/**
 * Given a squadron that arrived 2410 milliseconds ago
 * When it is painted
 * Then one missile flies
 */
TEST(Squadron, AShipFiresAtTheFireStart) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, JUST_AFTER_THE_FIRST_SHOT);

  EXPECT_THAT(cornersOf(paintingOf(squadron), MISSILE_SIZE), SizeIs(1));
}

/**
 * Given the first missile fired at 2.4 seconds, the next shot 1.6 seconds
 * later at the soonest
 * When 0.9 seconds have passed, time for it to cross the whole box
 * Then no missile is left
 */
TEST(Squadron, AMissileDisappearsAtTheRightEdgeOfTheBox) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, FIRST_MISSILE_OUT_NEXT_NOT_FIRED);

  EXPECT_THAT(cornersOf(paintingOf(squadron), MISSILE_SIZE), IsEmpty());
}

/**
 * Given a squadron for a system that asks for less motion, just built
 * When it is painted
 * Then the box is already open
 */
TEST(Squadron, ReducedMotionShowsTheBoxOpenAtOnce) {
  const Squadron squadron(MotionPreference::Reduced, ANY_SEED);

  const InterfaceCanvasRecord painted = paintingOf(squadron);

  EXPECT_THAT(painted.fills,
              Contains(RecordedFill{.area = {BOX.position, CORNER_STROKE},
                                    .color = EDGE_BLUE}));
}

/**
 * Given a squadron for a system that asks for less motion
 * When 10 seconds pass
 * Then its ships are still at their places, and none fired
 */
TEST(Squadron, ReducedMotionKeepsTheFormationStill) {
  Squadron squadron(MotionPreference::Reduced, ANY_SEED);

  play(squadron, A_WHILE);

  const InterfaceCanvasRecord painted = paintingOf(squadron);
  EXPECT_THAT(cornersOf(painted, SHIP_SIZE),
              ElementsAreArray(formationInBox()));
  EXPECT_THAT(cornersOf(painted, MISSILE_SIZE), IsEmpty());
}

/**
 * Given a squadron just built
 * When a single frame of one second is given, as after a stall
 * Then the squadron moved by MAXIMUM_BACKGROUND_STEP only: its line still grows
 */
TEST(Squadron, AStallMovesTheSquadronByOneStepOnly) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);

  squadron.advance(A_STALL);

  const InterfaceCanvasRecord painted = paintingOf(squadron);
  ASSERT_THAT(painted.fills, SizeIs(1));
  EXPECT_EQ(painted.fills.front().area.size.y, 1);
}

/**
 * Given a squadron whose four ships have landed, and the palette for
 * deuteranopia
 * When it is painted
 * Then the ships of players 2 to 4 swap their shades, and player 1 keeps its
 * blue
 */
TEST(Squadron, ShipsSwapTheirShadesButPlayerOnes) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, ONCE_LANDED_BEFORE_THE_DRIFT);

  const InterfaceCanvasRecord painted =
      paintingOf(squadron, DEUTERANOPIA_PLAYER_PALETTE);

  const auto swapped =
      std::ranges::count_if(painted.images, [](const RecordedImage& image) {
        return image.source.size == SHIP_SIZE && image.recolor.has_value();
      });
  const auto kept =
      std::ranges::count_if(painted.images, [](const RecordedImage& image) {
        return image.source.size == SHIP_SIZE && !image.recolor.has_value();
      });
  EXPECT_EQ(swapped, SHIPS_SWAPPED_IN_DEUTERANOPIA);
  EXPECT_EQ(kept, SHIPS_KEPT_IN_DEUTERANOPIA);
}

/**
 * Given a squadron whose four ships have landed, and the palette for
 * deuteranopia
 * When it is painted
 * Then the ship of player 3 swaps its greens for the bluish greens of the
 * palette
 */
TEST(Squadron, TheThirdShipTakesThePalettesShades) {
  Squadron squadron(MotionPreference::Full, ANY_SEED);
  play(squadron, ONCE_LANDED_BEFORE_THE_DRIFT);

  const InterfaceCanvasRecord painted =
      paintingOf(squadron, DEUTERANOPIA_PLAYER_PALETTE);

  EXPECT_TRUE(
      std::ranges::any_of(painted.images, [](const RecordedImage& image) {
        return image.source.position.y == THIRD_SHIP_ROW &&
               image.recolor == SHEET_GREEN_TO_BLUISH_GREEN;
      }));
}
