#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <optional>
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "QuitConfirmationScreen.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "WidgetConstants.hpp"

using rtype::client::PRESS_DURATION;
using rtype::client::QuitConfirmationScreen;
using ::testing::ElementsAre;

namespace {

/** @brief A quit confirmation alone, with a recording navigator and
 * canvas. */
class QuitConfirmationScreenTest : public ::testing::Test {
 protected:
  [[nodiscard]] const RecordingNavigator& navigator() const {
    return navigator_;
  }

  QuitConfirmationScreen& dialog() { return dialog_; }

  void tap(sf::Keyboard::Key key) {
    dialog_.handleEvent(keyPress(key));
    dialog_.handleEvent(keyRelease(key));
  }

 private:
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  QuitConfirmationScreen dialog_{
      navigator_, std::make_unique<RecordingInterfaceCanvas>(canvas_),
      samePlayfieldPoint};
};

}  // namespace

/**
 * Given the quit confirmation just opened
 * When Enter is pressed and released, and the press lasts
 * Then NO answers: the dialog asks to close itself
 */
TEST_F(QuitConfirmationScreenTest, NoIsChosenAtFirst) {
  tap(sf::Keyboard::Key::Enter);

  dialog().update(PRESS_DURATION);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the quit confirmation just opened
 * When Escape is pressed and released
 * Then NO answers at once
 */
TEST_F(QuitConfirmationScreenTest, EscapeAnswersNo) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the quit confirmation, NO selected
 * When the right arrow selects YES and Enter activates it
 * Then every screen is asked to close, which closes the client
 */
TEST_F(QuitConfirmationScreenTest, YesClosesTheClient) {
  dialog().handleEvent(keyPress(sf::Keyboard::Key::Right));
  tap(sf::Keyboard::Key::Enter);

  dialog().update(PRESS_DURATION);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::CloseAll,
                                        .screen = std::nullopt}));
}
