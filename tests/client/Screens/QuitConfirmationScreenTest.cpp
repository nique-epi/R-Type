#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "KeyPress.hpp"
#include "QuitConfirmationScreenFixture.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SoundConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::CLICK_SOUND_ID;
using rtype::client::PRESS_DURATION;
using ::testing::ElementsAre;

/**
 * Given the quit confirmation just opened
 * When Enter is pressed and released, and the press lasts
 * Then NO answers: the dialog asks to close itself
 */
TEST_F(QuitConfirmationScreenFixture, NoIsChosenAtFirst) {
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
TEST_F(QuitConfirmationScreenFixture, EscapeAnswersNo) {
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
TEST_F(QuitConfirmationScreenFixture, YesClosesTheClient) {
  dialog().handleEvent(keyPress(sf::Keyboard::Key::Right));
  tap(sf::Keyboard::Key::Enter);

  dialog().update(PRESS_DURATION);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::CloseAll,
                                        .screen = std::nullopt}));
}

/**
 * Given the quit confirmation
 * When Escape answers NO
 * Then the click of NO plays once
 */
TEST_F(QuitConfirmationScreenFixture, AnsweringPlaysTheClick) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}
