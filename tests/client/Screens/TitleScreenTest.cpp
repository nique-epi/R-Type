#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include "BlankRenderTarget.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "TitleScreen.hpp"
#include "WidgetConstants.hpp"

using rtype::client::LOGO_ID;
using rtype::client::MotionPreference;
using rtype::client::PRESS_DURATION;
using rtype::client::ScreenId;
using rtype::client::TitleScreen;
using ::testing::ElementsAre;

namespace {

constexpr std::uint32_t ANY_SEED = 7;
constexpr sf::Vector2i LOGO_TOP_LEFT(24, 36);

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
 * Then the game is asked for in place of every screen
 */
TEST_F(TitleScreenTest, PlayAsksForTheGame) {
  choose(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Game}));
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
