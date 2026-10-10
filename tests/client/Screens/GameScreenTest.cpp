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
using rtype::client::ScreenFactory;
using rtype::client::ScreenStack;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

constexpr const char* OPTIONS = "options";
constexpr sf::Keyboard::Key OTHER_KEY = sf::Keyboard::Key::Space;
constexpr int KEY_REPEATS = 3;

ScreenFactory gameScreen(ScreenStack& screens,
                         std::vector<JournalEntry>& journal) {
  return [&screens, &journal] {
    return std::make_unique<GameScreen>(screens,
                                        recordingScreen(OPTIONS, journal));
  };
}

}  // namespace

/**
 * Given the game alone in the stack
 * When the player releases the options key
 * Then the options screen is built
 */
TEST(GameScreen, ReleasingTheOptionsKeyOpensTheOptions) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(gameScreen(screens, journal));
  screens.applyPendingChanges();

  screens.handleEvent(keyRelease(OPTIONS_KEY));
  screens.applyPendingChanges();

  EXPECT_THAT(journal, ElementsAre(JournalEntry{.screen = OPTIONS,
                                                .call = ScreenCall::Built}));
}

/**
 * Given the options opened over the game
 * When the options leave the stack
 * Then the game is still there
 */
TEST(GameScreen, OptionsOpenOverTheGame) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(gameScreen(screens, journal));
  screens.applyPendingChanges();
  screens.handleEvent(keyRelease(OPTIONS_KEY));
  screens.applyPendingChanges();

  screens.pop();
  screens.applyPendingChanges();

  EXPECT_FALSE(screens.isEmpty());
}

/**
 * Given the game alone in the stack
 * When the player holds the options key down, the system repeating its press
 * Then no options screen is built
 */
TEST(GameScreen, HoldingTheOptionsKeyLeavesTheOptionsClosed) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(gameScreen(screens, journal));
  screens.applyPendingChanges();

  for (int repeat = 0; repeat < KEY_REPEATS; ++repeat) {
    screens.handleEvent(keyPress(OPTIONS_KEY));
    screens.applyPendingChanges();
  }

  EXPECT_THAT(journal, IsEmpty());
}

/**
 * Given the game alone in the stack
 * When the player releases another key
 * Then no options screen is built
 */
TEST(GameScreen, OtherKeysLeaveTheOptionsClosed) {
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.push(gameScreen(screens, journal));
  screens.applyPendingChanges();

  screens.handleEvent(keyRelease(OTHER_KEY));
  screens.applyPendingChanges();

  EXPECT_THAT(journal, IsEmpty());
}
