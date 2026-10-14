#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <optional>
#include <vector>
#include "InterfaceColors.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "Squadron.hpp"
#include "SquadronConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::EDGE_BLUE;
using rtype::client::MotionPreference;
using rtype::client::Squadron;
using rtype::client::SQUADRON_BOX_SIZE;
using rtype::client::SQUADRON_SHIPS;
using rtype::client::SquadronShip;
using rtype::engine::Duration;
using ::testing::Contains;
using ::testing::ElementsAreArray;
using ::testing::IsEmpty;
using ::testing::SizeIs;

namespace {

constexpr std::uint32_t ANY_SEED = 7;
constexpr sf::IntRect BOX({288, 64}, SQUADRON_BOX_SIZE);
constexpr Duration FRAME = std::chrono::milliseconds(10);

/**
 * @brief Moments of the arrival, from the moment the squadron is built, each
 * well inside one phase of SquadronConstants.hpp so a rounding of the float
 * time never moves it to the next one.
 */
constexpr Duration WHILE_THE_LINE_GROWS = std::chrono::milliseconds(90);
constexpr Duration ONCE_THE_BOX_IS_OPEN = std::chrono::milliseconds(500);
constexpr Duration THREE_LETTERS_IN = std::chrono::milliseconds(575);
constexpr Duration JUST_AFTER_THE_FIRST_ARRIVAL =
    std::chrono::milliseconds(610);
constexpr Duration ONCE_LANDED_BEFORE_THE_DRIFT =
    std::chrono::milliseconds(1850);
constexpr Duration JUST_BEFORE_THE_FIRST_SHOT = std::chrono::milliseconds(2390);
constexpr Duration JUST_AFTER_THE_FIRST_SHOT = std::chrono::milliseconds(2410);
constexpr Duration FIRST_MISSILE_OUT_NEXT_NOT_FIRED =
    std::chrono::milliseconds(3300);
constexpr Duration LONG_WATCH = std::chrono::seconds(30);
constexpr Duration A_WHILE = std::chrono::seconds(10);
constexpr Duration A_STALL = std::chrono::seconds(1);
constexpr sf::Vector2i SHIP_SIZE(32, 16);
constexpr sf::Vector2i MISSILE_SIZE(16, 6);
constexpr int MIDDLE_ROW = 72;
constexpr int BOTTOM_ROW = 143;
constexpr sf::Vector2i CORNER_STROKE(8, 1);
constexpr sf::Vector2i TITLE_POSITION(296, 72);

/** @brief Advances a squadron frame by frame, so no step is capped. */
void play(Squadron& squadron, Duration total) {
  for (Duration played{}; played < total; played += FRAME) {
    squadron.advance(FRAME);
  }
}

/** @returns What a squadron paints in BOX. */
InterfaceCanvasRecord paintingOf(const Squadron& squadron) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  squadron.paint(canvas, BOX.position);
  return record;
}

/** @returns The top-left corners of the images of a size, in drawing
 * order. */
std::vector<sf::Vector2i> cornersOf(const InterfaceCanvasRecord& record,
                                    sf::Vector2i size) {
  std::vector<sf::Vector2i> corners;
  for (const RecordedImage& image : record.images) {
    if (image.source.size == size) {
      corners.push_back(image.topLeft);
    }
  }
  return corners;
}

/** @returns The places of the four ships, in pixels of the picture. */
std::vector<sf::Vector2i> formationInBox() {
  std::vector<sf::Vector2i> places;
  places.reserve(SQUADRON_SHIPS.size());
  for (const SquadronShip& ship : SQUADRON_SHIPS) {
    places.push_back(BOX.position + sf::Vector2i(ship.place));
  }
  return places;
}

}  // namespace

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
