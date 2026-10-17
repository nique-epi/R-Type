#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>
#include "InterfaceColors.hpp"
#include "InterfaceConstants.hpp"
#include "KeyPress.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuPainting.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingRowControl.hpp"
#include "TextField.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"

using rtype::client::DIM_LAYER_COLOR;
using rtype::client::INTERFACE_AREA;
using rtype::client::Menu;
using rtype::client::MenuBuilder;
using rtype::client::RULE_COLOR;
using rtype::client::TextAlignment;
using rtype::client::TextField;
using rtype::client::TextFont;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::Not;

namespace {

constexpr sf::Vector2i LOGO_SIZE(210, 42);
constexpr sf::Vector2i SQUADRON_SIZE(168, 144);
constexpr std::size_t SIXTEEN_BYTES = 16;

/** @brief Size of the box of a field on the picture of the connection screen
 * the team chose, in pixels. */
constexpr sf::Vector2i FIELD_BOX(208, 16);

/** @brief The menu of the title screen of the mockup, without actions. */
Menu titleMenu() {
  return MenuBuilder(MenuBuilder::Frame::Page)
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
  return MenuBuilder(MenuBuilder::Frame::Dialog)
      .heading("CONFIRM")
      .message("QUESTION")
      .cancelButton({.label = "NO"}, [] {})
      .button({.label = "YES"}, [] {})
      .build();
}

/**
 * @brief The connection screen of the picture the team chose, without
 * actions: its heading, three fields, then CONNECT and BACK.
 */
Menu connectionForm() {
  return MenuBuilder(MenuBuilder::Frame::Form)
      .heading("DIRECT CONNECT")
      .textField({.label = "ADDRESS", .description = "> ADDRESS"},
                 TextField("192.168.1.20", SIXTEEN_BYTES,
                           TextField::Characters::DigitsAndDots),
                 [] {})
      .textField(
          {.label = "PORT"},
          TextField("4243", SIXTEEN_BYTES, TextField::Characters::Digits),
          [] {})
      .textField(
          {.label = "PILOT"},
          TextField("ACE", SIXTEEN_BYTES, TextField::Characters::Printable),
          [] {})
      .button({.label = "CONNECT"}, [] {})
      .cancelButton({.label = "BACK"}, [] {})
      .build();
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
  const InterfaceCanvasRecord painted = paintingOf(
      MenuBuilder(MenuBuilder::Frame::Dialog).heading("CONFIRM").build());

  ASSERT_EQ(painted.writings.size(), 1U);
  EXPECT_EQ(painted.writings.front().text, "CONFIRM");
}

/**
 * Given the connection screen of the picture the team chose
 * When it is built and painted
 * Then its heading is written at (136, 50), inside a panel of 240 by 194
 * centered on the picture
 */
TEST(MenuBuilder, FormWritesItsHeadingInItsCenteredPanel) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  EXPECT_EQ(anchorOf(painted, "DIRECT CONNECT"), sf::Vector2i(136, 50));
}

/**
 * Given the connection screen of the picture the team chose
 * When it is built and painted
 * Then the labels of its fields stack under the heading at rows 70, 100 and
 * 130
 */
TEST(MenuBuilder, FormStacksTheLabelsOfItsFields) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  EXPECT_EQ(anchorOf(painted, "ADDRESS"), sf::Vector2i(136, 70));
  EXPECT_EQ(anchorOf(painted, "PORT"), sf::Vector2i(136, 100));
  EXPECT_EQ(anchorOf(painted, "PILOT"), sf::Vector2i(136, 130));
}

/**
 * Given the connection screen of the picture the team chose
 * When it is built and painted
 * Then the box of each field lies under its label, 208 by 16, at rows 80,
 * 110 and 140
 */
TEST(MenuBuilder, FormSinksTheBoxOfEachFieldUnderItsLabel) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  EXPECT_THAT(
      painted.fills,
      Contains(Field(&RecordedFill::area, sf::IntRect({136, 80}, FIELD_BOX))));
  EXPECT_THAT(
      painted.fills,
      Contains(Field(&RecordedFill::area, sf::IntRect({136, 110}, FIELD_BOX))));
  EXPECT_THAT(
      painted.fills,
      Contains(Field(&RecordedFill::area, sf::IntRect({136, 140}, FIELD_BOX))));
}

/**
 * Given the connection screen of the picture the team chose
 * When it is built and painted
 * Then the text of each field is written four pixels inside its box
 */
TEST(MenuBuilder, FormWritesTheTextOfEachFieldInsideItsBox) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  EXPECT_EQ(anchorOf(painted, "192.168.1.20"), sf::Vector2i(140, 84));
  EXPECT_EQ(anchorOf(painted, "4243"), sf::Vector2i(140, 114));
  EXPECT_EQ(anchorOf(painted, "ACE"), sf::Vector2i(140, 144));
}

/**
 * Given the connection screen of the picture the team chose
 * When it is built and painted
 * Then CONNECT and BACK form a row centered in the panel, bars of 80 by 20 at
 * (156, 166) and (244, 166)
 */
TEST(MenuBuilder, FormCentersItsButtonsInARowUnderItsFields) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  EXPECT_EQ(anchorOf(painted, "CONNECT"), sf::Vector2i(168, 172));
  EXPECT_EQ(anchorOf(painted, "BACK"), sf::Vector2i(256, 172));
}

/**
 * Given the connection screen of the picture the team chose, its first field
 * selected
 * When it is built and painted
 * Then the description of the field is written under the buttons, at
 * (136, 196)
 */
TEST(MenuBuilder, FormWritesTheDescriptionUnderItsButtons) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  EXPECT_EQ(anchorOf(painted, "> ADDRESS"), sf::Vector2i(136, 196));
}

/**
 * Given the connection screen of the picture the team chose, with a status
 * line
 * When it is painted
 * Then the status line is written under the description, at (136, 212)
 */
TEST(MenuBuilder, FormWritesTheStatusUnderTheDescription) {
  Menu form = connectionForm();
  form.setStatus("WAIT");

  const InterfaceCanvasRecord painted = paintingOf(form);

  EXPECT_EQ(anchorOf(painted, "WAIT"), sf::Vector2i(136, 212));
}

/**
 * Given the connection screen of the picture the team chose
 * When it is built and painted
 * Then the label of a field is in small print
 */
TEST(MenuBuilder, FormWritesTheLabelsOfItsFieldsInSmallPrint) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  const RecordedWriting* label = writingOf(painted, "ADDRESS");
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->style.font, TextFont::Small);
}

/**
 * Given the connection screen of the picture the team chose
 * When it is built and painted
 * Then the description of the selected field is in small print
 */
TEST(MenuBuilder, FormWritesItsDescriptionInSmallPrint) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  const RecordedWriting* description = writingOf(painted, "> ADDRESS");
  ASSERT_NE(description, nullptr);
  EXPECT_EQ(description->style.font, TextFont::Small);
}

/**
 * Given a form
 * When it is painted
 * Then nothing dims the screens below
 */
TEST(MenuBuilder, FormLeavesTheScreensBelowAsTheyAre) {
  const InterfaceCanvasRecord painted = paintingOf(connectionForm());

  EXPECT_THAT(painted.fills,
              Not(Contains(RecordedFill{.area = INTERFACE_AREA,
                                        .color = DIM_LAYER_COLOR})));
}

/**
 * Given a menu with the back button
 * When Escape is pressed and released
 * Then the screen on top is asked to close
 */
TEST(MenuBuilder, BackButtonClosesTheScreenOnTop) {
  RecordingNavigator navigator;
  Menu menu =
      MenuBuilder(MenuBuilder::Frame::Page).backButton(navigator).build();

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

  const InterfaceCanvasRecord painted = paintingOf(
      MenuBuilder(MenuBuilder::Frame::Page).backButton(navigator).build());

  EXPECT_NE(writingOf(painted, "BACK"), nullptr);
}

namespace {

/** @brief Size of the chart of window sizes on the picture of the options. */
constexpr sf::Vector2i CHART_SIZE(128, 72);

/** @brief A tabbed panel of the options, without actions: one row, a chart
 * under it, BACK, over a game. */
Menu displayTab(RowControlRecord& record) {
  return MenuBuilder(MenuBuilder::Frame::TabbedPanel)
      .heading("SETTINGS")
      .tabs({"SOUND", "CONTROLS", "DISPLAY", "ACCESS"}, 2,
            {.previous = "Q", .next = "E"}, [](std::size_t) {})
      .row({.label = "WINDOW SIZE", .description = "> SIZES"},
           std::make_unique<RecordingRowControl>(
               record, std::vector<sf::Keyboard::Key>{}, true))
      .illustration(CHART_SIZE)
      .cancelButton({.label = "BACK"}, [] {})
      .notice("THE GAME GOES ON")
      .dimScreensBelow()
      .build();
}

}  // namespace

/**
 * Given the DISPLAY tab of the options
 * When it is painted
 * Then its row label is at (36, 74), 8 and 4 pixels into the first row of
 * the panel, as on the picture
 */
TEST(MenuBuilder, TheRowsOfATabbedPanelLieInItsPanel) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting = paintingOf(displayTab(record));
  const RecordedWriting* label = writingOf(painting, "WINDOW SIZE");
  ASSERT_NE(label, nullptr);
  EXPECT_EQ(label->anchor, sf::Vector2i(36, 74));
}

/**
 * Given the DISPLAY tab
 * When it is painted
 * Then BACK is written at (36, 230), on its bar at (24, 224) under the panel
 */
TEST(MenuBuilder, TheBarsOfATabbedPanelLieUnderItsPanel) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting = paintingOf(displayTab(record));
  const RecordedWriting* back = writingOf(painting, "BACK");
  ASSERT_NE(back, nullptr);
  EXPECT_EQ(back->anchor, sf::Vector2i(36, 230));
}

/**
 * Given the DISPLAY tab, its row selected
 * When it is painted
 * Then the description of the row is written at (112, 230), right of BACK
 */
TEST(MenuBuilder, TheDescriptionOfATabbedPanelIsBesideItsBars) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting = paintingOf(displayTab(record));
  const RecordedWriting* description = writingOf(painting, "> SIZES");
  ASSERT_NE(description, nullptr);
  EXPECT_EQ(description->anchor, sf::Vector2i(112, 230));
}

/**
 * Given the DISPLAY tab, its single row and its chart
 * When its illustration area is read
 * Then the chart lies at (36, 98), 128 by 72, under the row
 */
TEST(MenuBuilder, TheIllustrationOfATabbedPanelLiesUnderItsRows) {
  RowControlRecord record;
  EXPECT_EQ(displayTab(record).illustrationArea(),
            sf::IntRect({36, 98}, {128, 72}));
}

/**
 * Given the DISPLAY tab opened over a game
 * When it is painted
 * Then its notice is right-aligned at (456, 24), and the screens below are
 * dimmed first
 */
TEST(MenuBuilder, ATabbedPanelOverAGameSaysSoAndDimsIt) {
  RowControlRecord record;
  const InterfaceCanvasRecord painting = paintingOf(displayTab(record));
  const RecordedWriting* notice = writingOf(painting, "THE GAME GOES ON");
  ASSERT_NE(notice, nullptr);
  EXPECT_EQ(notice->anchor, sf::Vector2i(456, 24));
  EXPECT_EQ(notice->style.alignment, TextAlignment::Right);
  ASSERT_FALSE(painting.fills.empty());
  EXPECT_EQ(painting.fills.front().color, DIM_LAYER_COLOR);
}
