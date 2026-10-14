#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string_view>
#include "InterfaceColors.hpp"
#include "InterfaceConstants.hpp"
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuFrame.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"

using rtype::client::DIM_LAYER_COLOR;
using rtype::client::INTERFACE_AREA;
using rtype::client::Menu;
using rtype::client::MenuBuilder;
using rtype::client::MenuFrame;
using rtype::client::RULE_COLOR;
using rtype::client::TextAlignment;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Not;

namespace {

constexpr sf::Vector2i LOGO_SIZE(210, 42);
constexpr sf::Vector2i SQUADRON_SIZE(168, 144);

/** @brief The menu of the title screen of the mockup, without actions. */
Menu titleMenu() {
  return MenuBuilder(MenuFrame::Page)
      .version("VER")
      .logo("logo.png", LOGO_SIZE)
      .subtitle("SUBTITLE")
      .button({.label = "PLAY", .description = "> PLAY"}, [] {})
      .button({.label = "SETTINGS"}, [] {})
      .cancelButton({.label = "QUIT"}, [] {})
      .illustration(SQUADRON_SIZE)
      .footer("FOOTER")
      .build();
}

/** @brief The quit confirmation of the mockup, without actions. */
Menu confirmation() {
  return MenuBuilder(MenuFrame::Dialog)
      .heading("CONFIRM")
      .message("QUESTION")
      .cancelButton({.label = "NO"}, [] {})
      .button({.label = "YES"}, [] {})
      .build();
}

/** @returns What a menu paints. */
InterfaceCanvasRecord paintingOf(const Menu& menu) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  menu.paint(canvas);
  return record;
}

/** @returns Where the line with this text was anchored, or nothing. */
std::optional<sf::Vector2i> anchorOf(const InterfaceCanvasRecord& record,
                                     std::string_view text) {
  const RecordedWriting* writing = writingOf(record, text);
  return writing == nullptr ? std::nullopt
                            : std::optional<sf::Vector2i>(writing->anchor);
}

}  // namespace

/**
 * Given the parts of the title screen of the mockup
 * When the page is built and painted
 * Then the version, the logo and the subtitle stack down the left column at
 * rows 24, 36 and 86, as on the mockup
 */
TEST(MenuBuilder, PageStacksItsHeaderAsTheMockup) {
  const InterfaceCanvasRecord painted = paintingOf(titleMenu());

  EXPECT_EQ(anchorOf(painted, "VER"), sf::Vector2i(24, 24));
  ASSERT_EQ(painted.images.size(), 1U);
  EXPECT_EQ(painted.images.front().topLeft, sf::Vector2i(24, 36));
  EXPECT_EQ(anchorOf(painted, "SUBTITLE"), sf::Vector2i(24, 86));
}

/**
 * Given the parts of the title screen of the mockup
 * When the page is built and painted
 * Then a rule 240 pixels wide lies at row 100, under the subtitle
 */
TEST(MenuBuilder, PageDrawsARuleUnderItsHeader) {
  const InterfaceCanvasRecord painted = paintingOf(titleMenu());

  EXPECT_THAT(painted.fills,
              Contains(RecordedFill{.area = {{24, 100}, {240, 1}},
                                    .color = RULE_COLOR}));
}

/**
 * Given the parts of the title screen of the mockup
 * When the page is built and painted
 * Then its bars start at rows 110, 138 and 166, their labels 12 pixels in and
 * 6 pixels down
 */
TEST(MenuBuilder, PageStacksItsButtonsAsTheMockup) {
  const InterfaceCanvasRecord painted = paintingOf(titleMenu());

  EXPECT_EQ(anchorOf(painted, "PLAY"), sf::Vector2i(36, 116));
  EXPECT_EQ(anchorOf(painted, "SETTINGS"), sf::Vector2i(36, 144));
  EXPECT_EQ(anchorOf(painted, "QUIT"), sf::Vector2i(36, 172));
}

/**
 * Given the parts of the title screen of the mockup, PLAY selected
 * When the page is built and painted
 * Then the description of PLAY is written at row 194, under the last bar
 */
TEST(MenuBuilder, PageWritesTheDescriptionUnderItsButtons) {
  const InterfaceCanvasRecord painted = paintingOf(titleMenu());

  EXPECT_EQ(anchorOf(painted, "> PLAY"), sf::Vector2i(24, 194));
}

/**
 * Given the parts of the title screen of the mockup
 * When the page is built and painted
 * Then the footer ends against the right side of the safe margin, row 241
 */
TEST(MenuBuilder, PageAnchorsItsFooterInTheBottomRightCorner) {
  const InterfaceCanvasRecord painted = paintingOf(titleMenu());

  const RecordedWriting* footer = writingOf(painted, "FOOTER");
  ASSERT_NE(footer, nullptr);
  EXPECT_EQ(footer->anchor, sf::Vector2i(456, 241));
  EXPECT_EQ(footer->style.alignment, TextAlignment::Right);
}

/**
 * Given the parts of the title screen of the mockup
 * When the page is built
 * Then its illustration lies against the right side of the safe margin, its
 * top at row 64
 */
TEST(MenuBuilder, PageReservesItsIllustrationAgainstTheRightMargin) {
  EXPECT_EQ(titleMenu().illustrationArea(),
            sf::IntRect({288, 64}, SQUADRON_SIZE));
}

/**
 * Given a page
 * When it is painted
 * Then nothing dims the screens below
 */
TEST(MenuBuilder, PageLeavesTheScreensBelowAsTheyAre) {
  const InterfaceCanvasRecord painted = paintingOf(titleMenu());

  EXPECT_THAT(painted.fills,
              Not(Contains(RecordedFill{.area = INTERFACE_AREA,
                                        .color = DIM_LAYER_COLOR})));
}

/**
 * Given the quit confirmation of the mockup
 * When it is painted
 * Then the whole picture is dimmed before anything else
 */
TEST(MenuBuilder, DialogDimsTheScreensBelowFirst) {
  const InterfaceCanvasRecord painted = paintingOf(confirmation());

  ASSERT_FALSE(painted.fills.empty());
  EXPECT_EQ(painted.fills.front(),
            (RecordedFill{.area = INTERFACE_AREA, .color = DIM_LAYER_COLOR}));
}

/**
 * Given the quit confirmation of the mockup
 * When it is built and painted
 * Then its heading and its question are written inside the centered panel,
 * at (120, 99) and (120, 119)
 */
TEST(MenuBuilder, DialogWritesItsTextsInItsCenteredPanel) {
  const InterfaceCanvasRecord painted = paintingOf(confirmation());

  EXPECT_EQ(anchorOf(painted, "CONFIRM"), sf::Vector2i(120, 99));
  EXPECT_EQ(anchorOf(painted, "QUESTION"), sf::Vector2i(120, 119));
}

/**
 * Given the quit confirmation of the mockup
 * When it is built and painted
 * Then NO and YES form a row centered in the panel, bars at (160, 143) and
 * (256, 143)
 */
TEST(MenuBuilder, DialogCentersItsButtonsInARow) {
  const InterfaceCanvasRecord painted = paintingOf(confirmation());

  EXPECT_EQ(anchorOf(painted, "NO"), sf::Vector2i(172, 149));
  EXPECT_EQ(anchorOf(painted, "YES"), sf::Vector2i(268, 149));
}

/**
 * Given a dialog without any button
 * When it is built and painted
 * Then it writes its heading and no bar label
 */
TEST(MenuBuilder, DialogWithoutButtonsWritesOnlyItsTexts) {
  const InterfaceCanvasRecord painted =
      paintingOf(MenuBuilder(MenuFrame::Dialog).heading("CONFIRM").build());

  ASSERT_EQ(painted.writings.size(), 1U);
  EXPECT_EQ(painted.writings.front().text, "CONFIRM");
}

/**
 * Given a menu with the back button
 * When Escape is pressed and released
 * Then the screen on top is asked to close
 */
TEST(MenuBuilder, BackButtonClosesTheScreenOnTop) {
  RecordingNavigator navigator;
  Menu menu = MenuBuilder(MenuFrame::Page).backButton(navigator).build();

  menu.handleEvent(keyPress(sf::Keyboard::Key::Escape), {});
  menu.handleEvent(keyRelease(sf::Keyboard::Key::Escape), {});

  EXPECT_THAT(navigator.calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given a menu with the back button
 * When it is painted
 * Then the button is labelled BACK
 */
TEST(MenuBuilder, BackButtonIsLabelledBack) {
  RecordingNavigator navigator;

  const InterfaceCanvasRecord painted =
      paintingOf(MenuBuilder(MenuFrame::Page).backButton(navigator).build());

  EXPECT_NE(writingOf(painted, "BACK"), nullptr);
}
