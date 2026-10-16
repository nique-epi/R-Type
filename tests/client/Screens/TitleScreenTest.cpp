#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include <vector>
#include "BackgroundConstants.hpp"
#include "BlankRenderTarget.hpp"
#include "IScreen.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "QuitConfirmationScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingScreen.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "TitleScreen.hpp"
#include "WidgetConstants.hpp"

using rtype::client::BACKGROUND_PIXEL_SIZE;
using rtype::client::IScreen;
using rtype::client::LOGO_ID;
using rtype::client::MotionPreference;
using rtype::client::PRESS_DURATION;
using rtype::client::QuitConfirmationScreen;
using rtype::client::ScreenId;
using rtype::client::ScreenStack;
using rtype::client::TitleScreen;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

constexpr std::uint32_t ANY_SEED = 7;
constexpr sf::Vector2i LOGO_TOP_LEFT(24, 36);

/**
 * @brief The middle of the bar of PLAY on the mockup, whose bar lies at
 * (24, 110) and is 160 x 20 pixels large, in logical units of the playfield.
 */
constexpr sf::Vector2i PLAY_BAR_MIDDLE =
    sf::Vector2i(104, 120) * static_cast<int>(BACKGROUND_PIXEL_SIZE);

/** @brief A title screen alone, with a recording navigator and canvas. */
class TitleScreenTest : public ::testing::Test {
 protected:
  [[nodiscard]] const RecordingNavigator& navigator() const {
    return navigator_;
  }

  [[nodiscard]] const InterfaceCanvasRecord& canvas() const { return canvas_; }

  TitleScreen& title() { return title_; }

  /** @brief Presses and releases a key, then lets the press last. */
  void choose(sf::Keyboard::Key key) {
    title_.handleEvent(keyPress(key));
    title_.handleEvent(keyRelease(key));
    title_.update(PRESS_DURATION);
  }

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  TitleScreen title_{navigator_,
                     std::make_unique<RecordingInterfaceCanvas>(canvas_),
                     samePlayfieldPoint, MotionPreference::Full, ANY_SEED};
};

/**
 * @brief The title screen alone in a stack, with the screens it opens to put
 * over it: the real quit confirmation, and a recording screen that hides the
 * screens below for the options, since OptionsScreen needs a real window. The
 * title and the dialog ask their own recording navigators, so what they ask
 * for is written down and never applied.
 */
class TitleInTheStackTest : public ::testing::Test {
 protected:
  TitleInTheStackTest() {
    screens_.replaceAll(ScreenId::Title);
    screens_.applyPendingChanges();
  }

  [[nodiscard]] const RecordingNavigator& titleNavigator() const {
    return titleNavigator_;
  }

  [[nodiscard]] const InterfaceCanvasRecord& titleCanvas() const {
    return titleCanvas_;
  }

  ScreenStack& screens() { return screens_; }

  /** @brief Puts a screen over the title. */
  void open(ScreenId screen) {
    screens_.push(screen);
    screens_.applyPendingChanges();
  }

  /** @brief Closes the screen on top. */
  void closeTheScreenOnTop() {
    screens_.pop();
    screens_.applyPendingChanges();
  }

  /** @brief Presses and releases the left button at a pixel of the window,
   * then lets the press last. */
  void click(sf::Vector2i position) {
    screens_.handleEvent(leftPress(position));
    screens_.handleEvent(leftRelease(position));
    screens_.update(PRESS_DURATION);
  }

 private:
  std::unique_ptr<IScreen> build(ScreenId screen) {
    if (screen == ScreenId::Title) {
      return std::make_unique<TitleScreen>(
          titleNavigator_,
          std::make_unique<RecordingInterfaceCanvas>(titleCanvas_),
          samePlayfieldPoint, MotionPreference::Full, ANY_SEED);
    }
    if (screen == ScreenId::QuitConfirmation) {
      return std::make_unique<QuitConfirmationScreen>(
          dialogNavigator_,
          std::make_unique<RecordingInterfaceCanvas>(dialogCanvas_),
          samePlayfieldPoint);
    }
    auto options = std::make_unique<RecordingScreen>(screen, journal_);
    options->setHidesScreensBelow(true);
    return options;
  }

  RecordingNavigator titleNavigator_;
  InterfaceCanvasRecord titleCanvas_;
  RecordingNavigator dialogNavigator_;
  InterfaceCanvasRecord dialogCanvas_;
  std::vector<JournalEntry> journal_;
  ScreenStack screens_{[this](ScreenId screen) { return build(screen); }};
};

}  // namespace

/**
 * Given the title screen just built
 * When nothing happened yet
 * Then its picture already holds the logo, at its place on the mockup
 */
TEST_F(TitleScreenTest, TheFirstFrameAlreadyShowsTheLogo) {
  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, LOGO_ID);
  EXPECT_EQ(canvas().images.front().topLeft, LOGO_TOP_LEFT);
}

/**
 * Given the title screen, PLAY selected
 * When Enter is pressed and released, and the press lasts
 * Then the connection screen is asked for over the title
 */
TEST_F(TitleScreenTest, PlayOpensTheConnectionScreen) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Connection}));
}

/**
 * Given the title screen, PLAY selected
 * When the down arrow selects SETTINGS and Enter activates it
 * Then the options are asked for over the title
 */
TEST_F(TitleScreenTest, SettingsOpensTheOptionsOverTheTitle) {
  title().handleEvent(keyPress(sf::Keyboard::Key::Down));

  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Options}));
}

/**
 * Given the title screen, PLAY selected
 * When the up arrow wraps to QUIT and Enter activates it
 * Then the quit confirmation is asked for over the title
 */
TEST_F(TitleScreenTest, QuitOpensTheConfirmation) {
  title().handleEvent(keyPress(sf::Keyboard::Key::Up));

  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::QuitConfirmation}));
}

/**
 * Given the title screen, PLAY selected
 * When Escape is pressed and released
 * Then the quit confirmation is asked for at once
 */
TEST_F(TitleScreenTest, EscapeOpensTheConfirmation) {
  title().handleEvent(keyPress(sf::Keyboard::Key::Escape));
  title().handleEvent(keyRelease(sf::Keyboard::Key::Escape));

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::QuitConfirmation}));
}

/**
 * Given the title screen
 * When it is drawn
 * Then its picture is laid once
 */
TEST_F(TitleScreenTest, DrawingLaysItsPicture) {
  BlankRenderTarget target;

  title().draw(target);

  EXPECT_EQ(canvas().showCount, 1);
}

/**
 * Given the options over the title
 * When a frame is drawn
 * Then the picture of the title is not laid
 */
TEST_F(TitleInTheStackTest, TheOptionsHideTheTitle) {
  BlankRenderTarget target;
  open(ScreenId::Options);

  screens().draw(target);

  EXPECT_EQ(titleCanvas().showCount, 0);
}

/**
 * Given the quit confirmation over the title
 * When a frame is drawn
 * Then the picture of the title is laid under the dialog, once
 */
TEST_F(TitleInTheStackTest, TheQuitConfirmationLeavesTheTitleShown) {
  BlankRenderTarget target;
  open(ScreenId::QuitConfirmation);

  screens().draw(target);

  EXPECT_EQ(titleCanvas().showCount, 1);
}

/**
 * Given the options over the title
 * When the left button is pressed and released where the bar of PLAY lies on
 * the title, and the press lasts
 * Then the title asks for nothing
 */
TEST_F(TitleInTheStackTest, AClickInTheOptionsNeverPressesAButtonOfTheTitle) {
  open(ScreenId::Options);

  click(PLAY_BAR_MIDDLE);

  EXPECT_THAT(titleNavigator().calls(), IsEmpty());
}

/**
 * Given the options opened over the title, then closed
 * When the left button is pressed and released on the bar of PLAY, and the
 * press lasts
 * Then the title asks for the connection screen
 */
TEST_F(TitleInTheStackTest, OnceTheOptionsAreClosedAClickReachesTheTitle) {
  open(ScreenId::Options);
  closeTheScreenOnTop();

  click(PLAY_BAR_MIDDLE);

  EXPECT_THAT(titleNavigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ScreenId::Connection}));
}
