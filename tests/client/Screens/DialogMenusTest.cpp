#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "DialogMenusFixture.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SoundConstants.hpp"

using rtype::client::CLICK_SOUND_ID;
using ::testing::ElementsAre;

/**
 * Given the quit confirmation just opened
 * When Enter is pressed and released, and the press lasts
 * Then NO answers: the dialog asks to close itself
 */
TEST_F(DialogMenusFixture, NoIsChosenAtFirstOnTheQuitConfirmation) {
  openQuitConfirmation();

  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the quit confirmation just opened
 * When Escape is pressed and released
 * Then NO answers at once
 */
TEST_F(DialogMenusFixture, EscapeAnswersNoOnTheQuitConfirmation) {
  openQuitConfirmation();

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
TEST_F(DialogMenusFixture, YesClosesTheClient) {
  openQuitConfirmation();

  press(sf::Keyboard::Key::Right);
  tap(sf::Keyboard::Key::Enter);
  letThePressLast();

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::CloseAll,
                                        .screen = std::nullopt}));
}

/**
 * Given the quit confirmation
 * When Escape answers NO
 * Then the click of NO plays once
 */
TEST_F(DialogMenusFixture, AnsweringTheQuitConfirmationPlaysTheClick) {
  openQuitConfirmation();

  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}
