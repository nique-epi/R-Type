#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <vector>
#include "GameScreen.hpp"
#include "KeyPress.hpp"
#include "RecordingScreen.hpp"
#include "ScreenConstants.hpp"
#include "ScreenStack.hpp"

using rtype::client::GameScreen;
using rtype::client::OPTIONS_KEY;
using rtype::client::ScreenStack;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

constexpr const char* OPTIONS = "options";
constexpr sf::Keyboard::Key OTHER_KEY = sf::Keyboard::Key::Space;

}  // namespace

/**
 * Given the game alone in the stack
 * When the player presses the options key
 * Then the options screen is built over the game
 */
TEST(GameScreen, OptionsKeyOpensTheOptions) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push([&screens, &journal] {
    return std::make_unique<GameScreen>(screens,
                                        recordingScreen(OPTIONS, journal));
  });
  screens.applyPendingChanges();

  screens.handleEvent(keyPress(OPTIONS_KEY));
  screens.applyPendingChanges();

  EXPECT_THAT(journal, ElementsAre(JournalEntry{.screen = OPTIONS,
                                                .call = ScreenCall::Built}));
}

/**
 * Given the game alone in the stack
 * When the player presses another key
 * Then no options screen is built
 */
TEST(GameScreen, OtherKeysLeaveTheOptionsClosed) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push([&screens, &journal] {
    return std::make_unique<GameScreen>(screens,
                                        recordingScreen(OPTIONS, journal));
  });
  screens.applyPendingChanges();

  screens.handleEvent(keyPress(OTHER_KEY));
  screens.applyPendingChanges();

  EXPECT_THAT(journal, IsEmpty());
}
