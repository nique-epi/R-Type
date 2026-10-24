#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include "BackgroundColorConstants.hpp"
#include "EntitySpriteTestConstants.hpp"
#include "HangarBay.hpp"
#include "InterfaceColorConstants.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"

using rtype::client::BayMotion;
using rtype::client::BayPilot;
using rtype::client::BayReadiness;
using rtype::client::DEEP_BLUE;
using rtype::client::HangarBay;
using rtype::client::HUD_CYAN;
using rtype::client::MotionPreference;
using rtype::client::PALE_BLUE;
using rtype::client::STAR_GOLD;
using rtype::client::TextAlignment;
using rtype::client::TextFont;
using ::testing::Contains;

namespace {

/** @brief The second bay of the waiting room the team chose, and where its
 * parts lie: the nickname on its middle column at row 120, the ship 19 in and
 * 30 down, the inside of its cell, YOU at the end of the number row. */
constexpr sf::Vector2i BAY_TOP_LEFT(134, 50);
constexpr sf::IntRect BAY_AREA({134, 50}, {102, 112});
constexpr sf::Vector2i NICKNAME_ANCHOR(185, 120);
constexpr sf::Vector2i SHIP_TOP_LEFT(153, 80);
constexpr sf::IntRect CELL_INSIDE({143, 137}, {84, 14});
constexpr sf::Vector2i LOCAL_TAG_ANCHOR(228, 58);

/** @brief The two frames of the third ship color of the entity sheet, and the
 * enlargement of a ship in a bay. */
constexpr sf::IntRect THIRD_COLOR_FIRST_FRAME({0, 36}, {32, 16});
constexpr sf::IntRect THIRD_COLOR_SECOND_FRAME({34, 36}, {32, 16});
constexpr int TWICE = 2;
constexpr int THIRD_COLOR = 2;
constexpr int THIRD_PLAYER = 3;

/** @brief Three steps of the beacons, and the fourth beacon of the second
 * bay: 19 + 3 × 9 in, 47 down. */
constexpr rtype::engine::Duration THREE_BEACON_STEPS =
    std::chrono::milliseconds(600);
constexpr sf::IntRect FOURTH_BEACON({180, 97}, {2, 2});
constexpr sf::Vector2i BEACON_SQUARE(2, 2);

constexpr const char* NICKNAME = "CAPTAIN_STARDUST";

/** @returns The bay of player 3 at BAY_TOP_LEFT, with a pilot in this state,
 * or open. */
HangarBay bayOf(std::optional<BayReadiness> readiness,
                bool isLocalPlayer = false) {
  HangarBay bay{.topLeft = BAY_TOP_LEFT,
                .playerNumber = THIRD_PLAYER,
                .color = THIRD_COLOR,
                .pilot = std::nullopt};
  if (readiness.has_value()) {
    bay.pilot = BayPilot{.nickname = NICKNAME,
                         .readiness = *readiness,
                         .isLocalPlayer = isLocalPlayer};
  }
  return bay;
}

/** @returns What painting a bay asks of a canvas, its animations run for
 * @p elapsed. */
InterfaceCanvasRecord bayPaintingOf(const HangarBay& bay,
                                    rtype::engine::Duration elapsed,
                                    MotionPreference motion) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintHangarBay(
      canvas, bay, BayMotion{.elapsed = elapsed, .preference = motion});
  return record;
}

/** @returns How many beacons of a record are of this color: fills of this
 * color as large as a beacon, unlike the strokes of the box around them. */
std::size_t beaconsOfColor(const InterfaceCanvasRecord& record,
                           sf::Color color) {
  return static_cast<std::size_t>(std::ranges::count_if(
      record.fills, [color](const RecordedFill& fill) noexcept {
        return fill.color == color && fill.area.size == BEACON_SQUARE;
      }));
}

}  // namespace

/**
 * Given a bay with a pilot
 * When it is painted
 * Then the nickname is centered on the middle column, row 120, in small print
 */
TEST(HangarBay, ThePilotsNicknameIsCenteredInSmallPrint) {
  const InterfaceCanvasRecord painted =
      bayPaintingOf(bayOf(BayReadiness::NotReady), {}, MotionPreference::Full);

  const RecordedWriting* nickname = writingOf(painted, NICKNAME);
  ASSERT_NE(nickname, nullptr);
  EXPECT_EQ(nickname->anchor, NICKNAME_ANCHOR);
  EXPECT_EQ(nickname->style.font, TextFont::Small);
  EXPECT_EQ(nickname->style.alignment, TextAlignment::Center);
}

/**
 * Given the bay of player 3
 * When it is painted
 * Then the third ship of the sheet is drawn twice its size, centered, inside
 * the bay
 */
TEST(HangarBay, ThePilotsShipIsTwiceItsSizeInItsColor) {
  const InterfaceCanvasRecord painted =
      bayPaintingOf(bayOf(BayReadiness::NotReady), {}, MotionPreference::Full);

  ASSERT_EQ(painted.images.size(), 1U);
  EXPECT_EQ(painted.images.front().source, THIRD_COLOR_FIRST_FRAME);
  EXPECT_EQ(painted.images.front().topLeft, SHIP_TOP_LEFT);
  EXPECT_EQ(painted.images.front().scale, TWICE);
  EXPECT_EQ(painted.images.front().visibleArea, BAY_AREA);
}

/**
 * Given a ready pilot
 * When one frame of the engines has passed
 * Then the ship shows its second frame: its engines run
 */
TEST(HangarBay, AReadyPilotsEnginesRun) {
  const InterfaceCanvasRecord painted =
      bayPaintingOf(bayOf(BayReadiness::Ready), SHIP_FRAME_TIME_LISTED,
                    MotionPreference::Full);

  ASSERT_EQ(painted.images.size(), 1U);
  EXPECT_EQ(painted.images.front().source, THIRD_COLOR_SECOND_FRAME);
}

/**
 * Given a pilot not ready
 * When one frame of the engines has passed
 * Then the ship still shows its first frame
 */
TEST(HangarBay, APilotNotReadyStaysStill) {
  const InterfaceCanvasRecord painted =
      bayPaintingOf(bayOf(BayReadiness::NotReady), SHIP_FRAME_TIME_LISTED,
                    MotionPreference::Full);

  ASSERT_EQ(painted.images.size(), 1U);
  EXPECT_EQ(painted.images.front().source, THIRD_COLOR_FIRST_FRAME);
}

/**
 * Given a ready pilot, and a system that asks for less motion
 * When one frame of the engines has passed
 * Then the ship still shows its first frame
 */
TEST(HangarBay, WithLessMotionAReadyShipStaysStill) {
  const InterfaceCanvasRecord painted =
      bayPaintingOf(bayOf(BayReadiness::Ready), SHIP_FRAME_TIME_LISTED,
                    MotionPreference::Reduced);

  ASSERT_EQ(painted.images.size(), 1U);
  EXPECT_EQ(painted.images.front().source, THIRD_COLOR_FIRST_FRAME);
}

/**
 * Given a ready pilot
 * When the bay is painted
 * Then its cell says READY on a pale fill
 */
TEST(HangarBay, AReadyPilotsCellSaysReadyOnAPaleFill) {
  const InterfaceCanvasRecord painted =
      bayPaintingOf(bayOf(BayReadiness::Ready), {}, MotionPreference::Full);

  EXPECT_NE(writingOf(painted, "READY"), nullptr);
  EXPECT_THAT(painted.fills,
              Contains(RecordedFill{.area = CELL_INSIDE, .color = PALE_BLUE}));
}

/**
 * Given a local player who asked to be ready, not shown ready yet
 * When the bay is painted
 * Then its cell says READY on gold
 */
TEST(HangarBay, AnAskNotShownYetIsWrittenOnGold) {
  const InterfaceCanvasRecord painted = bayPaintingOf(
      bayOf(BayReadiness::ReadyAsked, true), {}, MotionPreference::Full);

  EXPECT_NE(writingOf(painted, "READY"), nullptr);
  EXPECT_THAT(painted.fills,
              Contains(RecordedFill{.area = CELL_INSIDE, .color = STAR_GOLD}));
}

/**
 * Given the bay of the local player
 * When it is painted
 * Then YOU ends at column 228, on the row of the number
 */
TEST(HangarBay, TheLocalPlayerIsTaggedYou) {
  const InterfaceCanvasRecord painted = bayPaintingOf(
      bayOf(BayReadiness::NotReady, true), {}, MotionPreference::Full);

  const RecordedWriting* tag = writingOf(painted, "YOU");
  ASSERT_NE(tag, nullptr);
  EXPECT_EQ(tag->anchor, LOCAL_TAG_ANCHOR);
  EXPECT_EQ(tag->style.alignment, TextAlignment::Right);
}

/**
 * Given an open bay
 * When three steps of the beacons have passed
 * Then the fourth beacon alone is lit
 */
TEST(HangarBay, AnOpenBayLightsTheBeaconOfItsStep) {
  const InterfaceCanvasRecord painted = bayPaintingOf(
      bayOf(std::nullopt), THREE_BEACON_STEPS, MotionPreference::Full);

  EXPECT_THAT(painted.fills,
              Contains(RecordedFill{.area = FOURTH_BEACON, .color = HUD_CYAN}));
  EXPECT_EQ(beaconsOfColor(painted, HUD_CYAN), 1U);
}

/**
 * Given an open bay, and a system that asks for less motion
 * When three steps of the beacons have passed
 * Then no beacon is lit
 */
TEST(HangarBay, WithLessMotionNoBeaconLights) {
  const InterfaceCanvasRecord painted = bayPaintingOf(
      bayOf(std::nullopt), THREE_BEACON_STEPS, MotionPreference::Reduced);

  EXPECT_EQ(beaconsOfColor(painted, HUD_CYAN), 0U);
  EXPECT_EQ(beaconsOfColor(painted, DEEP_BLUE), 8U);
}

/**
 * Given an open bay
 * When it is painted
 * Then it says OPEN BAY on the row of the nickname, and draws no ship
 */
TEST(HangarBay, AnOpenBaySaysSoAndDrawsNoShip) {
  const InterfaceCanvasRecord painted =
      bayPaintingOf(bayOf(std::nullopt), {}, MotionPreference::Full);

  EXPECT_EQ(anchorOf(painted, "OPEN BAY"), NICKNAME_ANCHOR);
  EXPECT_TRUE(painted.images.empty());
}
